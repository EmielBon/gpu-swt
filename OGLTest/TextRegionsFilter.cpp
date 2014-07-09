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
    //writeIDs                = LoadScreenSpaceProgram("WriteIDs");
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

void TextRegionsFilter::FindLetterCandidates(Ptr<Texture> input, GradientDirection gradientDirection, Ptr<Texture> output)
{
    auto swt = input->GetEmptyClone();
    
    swtFilter->Input = input;
    swtFilter->GradientDirection = gradientDirection;
    ApplyFilter(*swtFilter, swt);
    connectedComponentsFilter->Input = swt;
    ApplyFilter(*connectedComponentsFilter, output);
}

void TextRegionsFilter::FilterInvalidComponents(Ptr<Texture> input, Ptr<Texture> output)
{
    filterInvalidComponents->Use();
    filterInvalidComponents->Uniforms["Texture"].SetValue(*input);
    filterInvalidComponents->Uniforms["MinAspectRatio"].SetValue(MinAspectRatio);
    filterInvalidComponents->Uniforms["MaxAspectRatio"].SetValue(MaxAspectRatio);
    filterInvalidComponents->Uniforms["MinSizeRatio"].SetValue(0.0005f);
    filterInvalidComponents->Uniforms["MaxSizeRatio"].SetValue(0.02f);
    RenderToTexture(output);
}

void TextRegionsFilter::BoundingBoxes(Ptr<Texture> input, Ptr<Texture> output, bool clear)
{
    boundingBoxes->Use();
    boundingBoxes->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::CountComponents(Ptr<Texture> input, Ptr<Texture> output)
{
    countComponents->Use();
    countComponents->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
}

void TextRegionsFilter::Occupancy(Ptr<Texture> input, Ptr<Texture> output, bool clear)
{
    calculateOccupancy->Use();
    calculateOccupancy->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, clear ? GL_COLOR_BUFFER_BIT : 0);
}

void TextRegionsFilter::AverageColorAndSWT(Ptr<Texture> components, Ptr<Texture> occupancy, Ptr<Texture> inputImage, Ptr<Texture> swt, Ptr<Texture> output, bool clear)
{
    average->Use();
    average->Uniforms["Texture"].SetValue(*components);
    average->Uniforms["Occupancy"].SetValue(*occupancy);
    average->Uniforms["InputImage"].SetValue(*inputImage);
    average->Uniforms["SWT"].SetValue(*swt);
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
    
    glPointSize(N);
}

void TextRegionsFilter::ExtractBoundingBoxes(int N, int count)
{
    auto pixels = FrameBuffer::GetCurrentlyBound()->ReadPixels<cv::Vec4f>(0, 0, N, N, GL_RGBA, GL_FLOAT);
    for(int i = 0; i < count; ++i)
    {
        auto &pixel = pixels[i];
        int x1 = -((int)pixel[0] - Input->GetWidth() - 1);
        int y1 = -((int)pixel[1] - Input->GetHeight() - 1);
        int x2 = (int)pixel[2];
        int y2 = (int)pixel[3];
        //if (x1 != x2 && y1 != y2 && x2 != 0 && y2 != 0)
        ExtractedBoundingBoxes.push_back(BoundingBox(cv::Rect(x1, y1, x2 - x1, y2 - y1)));
    }
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
    ReserveColorBuffers(9);
    
    auto swt1        = ColorBuffers[0];
    auto swt2        = ColorBuffers[1];
    auto components1 = ColorBuffers[2];
    auto components2 = ColorBuffers[3];
    auto bboxes      = ColorBuffers[4];
    auto filtered    = ColorBuffers[5];
    auto occupancy   = ColorBuffers[6];
    auto averages    = ColorBuffers[7];
    auto temp        = ColorBuffers[8];
    
    //FindLetterCandidates(gray, GradientDirection::With,    components1);
    //FindLetterCandidates(gray, GradientDirection::Against, components2);
    
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
    DEBUG_FB("Average component colors");
    
    // Compute bounding boxes
    PrepareBoundingBoxCalculation();
    BoundingBoxes(components1, bboxes, true);
    BoundingBoxes(components2, bboxes, false);
    DEBUG_FB("Bounding Boxes");
    
    // End summations
    GraphicsDevice::UseDefaultBuffers();
    glDisable(GL_BLEND);
    
    // Filter invalid components
    FilterInvalidComponents(bboxes, filtered);
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