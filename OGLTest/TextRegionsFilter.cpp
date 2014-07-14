//
//  TextRegionsFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 23-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "TextRegionsFilter.h"
#include "GrayFilter.h"
#include "SWTFilter.h"
#include "ConnectedComponentsFilter.h"
#include "Texture.h"
#include "FrameBuffer.h"

#include "VertexPosition.h"
#include "VertexBuffer.h"
#include "GraphicsDevice.h"
#include "RenderWindow.h"

#include "SWTParameters.h"
#include <cmath>

void TextRegionsFilter::LoadShaderPrograms()
{
    grayFilter                = New<GrayFilter>(Input);
    swtFilter                 = New<SWTFilter>();
    connectedComponentsFilter = New<ConnectedComponentsFilter>();
    
    grayFilter->DoLoadShaderPrograms();
    swtFilter->DoLoadShaderPrograms();
    connectedComponentsFilter->DoLoadShaderPrograms();
    
    boundingBoxes           = LoadProgram("ScatterToRoot", "BoundingBoxes");
    filterInvalidComponents = LoadScreenSpaceProgram("FilterInvalidComponents");
    countComponents         = LoadProgram("CountComponents");
    stencilRouting          = LoadProgram("StencilRouting");
    calculateOccupancy      = LoadProgram("ScatterToRoot", "Occupancy");
    average                 = LoadProgram("ScatterToRoot", "AverageColorAndSWT");
    variance                = LoadProgram("ScatterToRoot", "Variance");
    //writeIDs              = LoadScreenSpaceProgram("WriteIDs");
    //vertexTexture = LoadProgram("VertexTexture");
}

void TextRegionsFilter::Initialize()
{
    gray = GetColorAttachment()->GetEmptyClone();
    ApplyFilter(*grayFilter, gray); DEBUG_FB("Gray");
    swtFilter->Input = gray;
    
    PreparePerPixelVertices();
}

void TextRegionsFilter::PrepareBoundingBoxCalculation()
{
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
}
void TextRegionsFilter::PrepareSummationOperations()
{
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

void TextRegionsFilter::FilterInvalidComponents(Ptr<Texture> boundingBoxes, Ptr<Texture> averages, Ptr<Texture> occupancy, Ptr<Texture> variances, Ptr<Texture> output)
{
    filterInvalidComponents->Use();
    filterInvalidComponents->Uniforms["BoundingBoxes"].SetValue(*boundingBoxes);
    filterInvalidComponents->Uniforms["Averages"].SetValue(*averages);
    filterInvalidComponents->Uniforms["Occupancy"].SetValue(*occupancy);
    filterInvalidComponents->Uniforms["Variances"].SetValue(*variances);
    RenderToTexture(output);
}

void TextRegionsFilter::BoundingBoxes(Ptr<Texture> components, Ptr<Texture> output, bool clear)
{
    boundingBoxes->Use();
    boundingBoxes->Uniforms["Components"].SetValue(*components);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::CountComponents(Ptr<Texture> input, Ptr<Texture> output)
{
    countComponents->Use();
    countComponents->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
}

void TextRegionsFilter::Occupancy(Ptr<Texture> components, Ptr<Texture> output, bool clear)
{
    calculateOccupancy->Use();
    calculateOccupancy->Uniforms["Components"].SetValue(*components);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::AverageColorAndSWT(Ptr<Texture> components, Ptr<Texture> occupancy, Ptr<Texture> inputImage, Ptr<Texture> swt, Ptr<Texture> output, bool clear)
{
    average->Use();
    average->Uniforms["Components"].SetValue(*components);
    average->Uniforms["Occupancy"].SetValue(*occupancy);
    average->Uniforms["InputImage"].SetValue(*inputImage);
    average->Uniforms["StrokeWidths"].SetValue(*swt);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::Variance(Ptr<Texture> components, Ptr<Texture> occupancy, Ptr<Texture> strokeWidths, Ptr<Texture> averages, Ptr<Texture> output, bool clear)
{
    variance->Use();
    variance->Uniforms["Components"].SetValue(*components);
    variance->Uniforms["Occupancy"].SetValue(*occupancy);
    variance->Uniforms["StrokeWidths"].SetValue(*strokeWidths);
    variance->Uniforms["Averages"].SetValue(*averages);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::PrepareStencilRouting(int N)
{
    int width  = Input->GetWidth();
    int height = Input->GetHeight();
    
    List<GLuint> pixels(N * N, 1);
    for(int i = 1; i < N * N; ++i)
        pixels[i] = pixels[i-1] + 1;
    
    stencil = New<Texture>(GL_DEPTH_STENCIL, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    FrameBuffer::GetCurrentlyBound()->SetDepthStencil(stencil);
    
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_DECR, GL_DECR, GL_DECR);
    
    stencil->Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, N, N, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, pixels.data());
    
    glPointSize(N + 1); // todo: why is +1 needed? Or maybe it just needs to be an even number, in the test case it was 7 and the first column of pixels was not drawn
}

void TextRegionsFilter::ExtractBoundingBoxes(int N, int count)
{
    auto pixels = FrameBuffer::GetCurrentlyBound()->ReadPixels<cv::Vec4f>(0, 0, N, N, GL_RGBA, GL_FLOAT);
    
    for(auto &pixel : pixels)
    {
        int x1 = -((int)pixel[0] - (Input->GetWidth() - 1));
        int y1 = -((int)pixel[1] - (Input->GetHeight() - 1));
        int x2 = (int)pixel[2];
        int y2 = (int)pixel[3];
        if (x2 - x1 < 0 || y2 - y1 < 0)
            continue;
        ExtractedBoundingBoxes.push_back(BoundingBox(cv::Rect(x1, y1, x2 - x1, y2 - y1)));
    }
    //if (ExtractedBoundingBoxes.size() != count)
    //    throw std::runtime_error("Error: Missing some bounding boxes");
}

void TextRegionsFilter::StencilRouting(Ptr<Texture> input, float N, Ptr<Texture> output)
{
    stencilRouting->Use();
    stencilRouting->Uniforms["Texture"].SetValue(*input);
    stencilRouting->Uniforms["StencilCenter"].SetValue(Vector2(N / 2, N / 2));
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
}

/*void TextRegionsFilter::WriteIDs(Ptr<Texture> input, Ptr<Texture> output)
{
    writeIDs->Use();
    writeIDs->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output);
}*/

void TextRegionsFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(10);
    
    auto swt1        = ColorBuffers[0];
    auto swt2        = ColorBuffers[1];
    auto components1 = ColorBuffers[2];
    auto components2 = ColorBuffers[3];
    auto bboxes      = ColorBuffers[4];
    auto filtered    = ColorBuffers[5];
    auto occupancy   = ColorBuffers[6];
    auto averages    = ColorBuffers[7];
    auto variances   = ColorBuffers[8];
    auto temp        = ColorBuffers[9];
    
    // Calculate SWT
    swtFilter->Input = gray;
    swtFilter->GradientDirection = GradientDirection::With;
    ApplyFilter(*swtFilter, swt1);
    
    swtFilter->GradientDirection = GradientDirection::Against;
    ApplyFilter(*swtFilter, swt2);
    
    // Determine connected components
    connectedComponentsFilter->Input = swt1;
    ApplyFilter(*connectedComponentsFilter, components1);
    connectedComponentsFilter->Input = swt2;
    ApplyFilter(*connectedComponentsFilter, components2);

    // Prepare summations
    PrepareSummationOperations();
    GraphicsDevice::SetBuffers(PerPixelVertices, nullptr);
    glEnable(GL_BLEND);
    
    // Calculate component occupancy
    Occupancy(components1, occupancy, true);
    Occupancy(components2, occupancy, false);
    DEBUG_FB("Component occupancy");
    
    // Average component color and SWT
    AverageColorAndSWT(components1, occupancy, Input, swt1, averages, true);
    AverageColorAndSWT(components2, occupancy, Input, swt2, averages, false);
    
    /*auto pixels = FrameBuffer::GetCurrentlyBound()->ReadPixels<cv::Vec4f>(0, 0, 800, 600, GL_RGBA, GL_FLOAT);
    for(auto& pixel : pixels)
    {
        if (pixel[3] != 0.0)
            printf("%f ", pixel[3]);
    }*/
    
    DEBUG_FB("Average component colors");
    
    // Calculate variance
    Variance(components1, occupancy, swt1, averages, variances, true);
    Variance(components2, occupancy, swt2, averages, variances, false);
    DEBUG_FB("Component variances");
    
    // Compute bounding boxes
    PrepareBoundingBoxCalculation();
    BoundingBoxes(components1, bboxes, true);
    BoundingBoxes(components2, bboxes, false);
    DEBUG_FB("Bounding Boxes");
    
    // End summations
    GraphicsDevice::UseDefaultBuffers();
    glDisable(GL_BLEND);
    
    // Filter invalid components
    FilterInvalidComponents(bboxes, averages, occupancy, variances, filtered);
    DEBUG_FB("Valids");
    
    // Count unique components
    GraphicsDevice::SetBuffers(PerPixelVertices, nullptr);
    PrepareSummationOperations();
    glEnable(GL_BLEND);
    CountComponents(filtered, temp);
    int componentCount = (int)FrameBuffer::ReadPixel(1, 1)[0];
    int N = (int)ceil(sqrt(componentCount));
    printf("Number of unique components: %i\n", componentCount);
    if (componentCount > 255)
        printf("Warning: Component count > 255\n");
    glDisable(GL_BLEND);
    
    // Stencil routing
    PrepareStencilRouting(N);
    glEnable(GL_STENCIL_TEST);
    StencilRouting(filtered, N, output);
    ExtractBoundingBoxes(N, componentCount);
    glDisable(GL_STENCIL_TEST);
    glPointSize(1);

    GraphicsDevice::UseDefaultBuffers();
}