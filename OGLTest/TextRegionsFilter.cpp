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
    
    boundingBoxes           = LoadProgram("BoundingBoxes");
    filterInvalidComponents = LoadScreenSpaceProgram("FilterInvalidComponents");
    countComponents         = LoadProgram("CountComponents");
    stencilRouting          = LoadProgram("StencilRouting");
    //vertexTexture = LoadProgram("VertexTexture");
}

void TextRegionsFilter::Initialize()
{
    gray = GetColorAttachment()->GetEmptyClone();
    ApplyFilter(*grayFilter, gray); DEBUG_FB("Gray");
    
    PreparePerPixelVertices();
}

void TextRegionsFilter::PrepareBoundingBoxCalculation()
{
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
}
void TextRegionsFilter::PrepareComponentCounting()
{
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

void TextRegionsFilter::PreparePerPixelVertices()
{
    int width  = Input->GetWidth();
    int height = Input->GetHeight();
    
    List<VertexPosition> vertices;
    for (int x = 0; x < width;  ++x)
        for (int y = 0; y < height; ++y)
            vertices.push_back(VertexPosition(Vector3(x, y, 0)));
    
    perPixelVertices = New<VertexBuffer>();
    perPixelVertices->SetData(vertices);
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

void TextRegionsFilter::ExtractBoundingBoxes(int N)
{
    List<cv::Vec4f> pixels(N * N);
    glReadPixels(0, 0, N, N, GL_RGBA, GL_FLOAT, pixels.data());
    for(auto& pixel : pixels)
    {
        int x1 = -((int)pixel[0] - 799);
        int y1 = -((int)pixel[1] - 599);
        int x2 = (int)pixel[2];
        int y2 = (int)pixel[3];
        if (x1 != x2 && y1 != y2 && x2 != 0 && y2 != 0)
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

void TextRegionsFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(6);
    
    auto components1 = ColorBuffers[0];
    auto components2 = ColorBuffers[1];
    auto bboxes      = ColorBuffers[2];
    auto filtered    = ColorBuffers[3];
    auto swt1        = ColorBuffers[4];
    auto swt2        = ColorBuffers[5];
    
    swtFilter->Input = gray;
    swtFilter->GradientDirection = GradientDirection::With;
    ApplyFilter(*swtFilter, swt1);
    connectedComponentsFilter->Input = swt1;
    ApplyFilter(*connectedComponentsFilter, components1);
    
    swtFilter->Input = gray;
    swtFilter->GradientDirection = GradientDirection::Against;
    ApplyFilter(*swtFilter, swt2);
    connectedComponentsFilter->Input = swt2;
    ApplyFilter(*connectedComponentsFilter, components2);
    
    //FindLetterCandidates(gray, GradientDirection::With,    components1);
    //FindLetterCandidates(gray, GradientDirection::Against, components2);
    
    // Compute bounding boxes
    PrepareBoundingBoxCalculation();
    GraphicsDevice::SetBuffers(perPixelVertices, nullptr);
    glEnable(GL_BLEND);
    BoundingBoxes(components1, bboxes, true);
    BoundingBoxes(components2, bboxes, false);
    glDisable(GL_BLEND);
    DEBUG_FB("BBoxes");
    
    GraphicsDevice::UseDefaultBuffers();
    
    FilterInvalidComponents(bboxes, filtered);
    DEBUG_FB("Valids");
    
    // Count unique components
    GraphicsDevice::SetBuffers(perPixelVertices, nullptr);
    PrepareComponentCounting();
    glEnable(GL_BLEND);
    CountComponents(filtered, ColorBuffers[1]);
    glDisable(GL_BLEND);
    int componentCount = (int)FrameBuffer::ReadPixel(1, 1)[0];
    int N = (int)ceil(sqrt(componentCount));
    printf("Number of unique components: %i\n", componentCount);
    if (componentCount > 255)
        printf("Warning: Component count > 255\n");
    
    // Stencil routing
    PrepareStencilRouting(N);
    glEnable(GL_STENCIL_TEST);
    StencilRouting(filtered, N, output);
    glDisable(GL_STENCIL_TEST);
    glPointSize(1);
    
    ExtractBoundingBoxes(N);
    
    GraphicsDevice::UseDefaultBuffers();
}