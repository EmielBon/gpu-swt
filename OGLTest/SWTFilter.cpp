//
//  SWTFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTFilter.h"
#include "VertexPositionTexture.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "SobelFilter.h"
#include "CannyFilter.h"
#include "RenderWindow.h"
#include "GaussianFilter.h"

void SWTFilter::LoadShaderPrograms()
{
    sobel    = New<SobelFilter>(Input);
    gaussian = New<GaussianFilter>(Input);
    canny    = New<CannyFilter>();
    
    cast  = LoadScreenSpaceProgram("StrokeWidthTransform1");
    write =            LoadProgram("StrokeWidthTransform2");
    avg   = LoadScreenSpaceProgram("StrokeWidthTransform3");
    scale = LoadScreenSpaceProgram("ScaleColor");
}

void SWTFilter::Initialize()
{
    gradients    = ApplyFilter(*sobel);
    canny->Input = ApplyFilter(*gaussian);
    edges        = ApplyFilter(*canny); // Builds a edge-only stencil buffer
    
    PrepareEdgeOnlyStencil();
    PrepareMaximizingDepthTest();
    PrepareRayLines(*edges);
    
    glClearColor(0, 0, 0, 1);
}

Ptr<Texture> SWTFilter::PerformSteps()
{
    if (GradientDirection == GradientDirection::Unspecified)
        throw std::runtime_error("Unspecified gradient direction in SWT step");
    
    auto quadVertices = GraphicsDevice::VertexBuffer;
    auto quadIndices  = GraphicsDevice::IndexBuffer;
    
    bool darkOnLight = (GradientDirection == GradientDirection::With);
    
    glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    auto values = CastRays(darkOnLight);
    glDisable(GL_STENCIL_TEST);
    
    GraphicsDevice::SetBuffers(linesVertices, nullptr);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto swt = WriteRayValues(*values, *values, darkOnLight);
    glDisable(GL_DEPTH_TEST);
    GraphicsDevice::SetBuffers(quadVertices, quadIndices);
    
    glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    auto valuesAvg = AverageRayValues(*swt, darkOnLight);
    glDisable(GL_STENCIL_TEST);
    
    GraphicsDevice::SetBuffers(linesVertices, nullptr);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto swtAvg = WriteRayValues(*valuesAvg, *values, darkOnLight);
    glDisable(GL_DEPTH_TEST);
    GraphicsDevice::SetBuffers(quadVertices, quadIndices);
    
    return swtAvg;
}

Ptr<Texture> SWTFilter::CastRays(bool darkOnLight)
{
    cast->Use();
    cast->Uniforms["Edges"].SetValue(*edges);
    cast->Uniforms["Gradients"].SetValue(*gradients);
    cast->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto result = Render();
    RenderWindow::Instance().AddTexture(result, "SWT values");
    return result;
}

Ptr<Texture> SWTFilter::WriteRayValues(const Texture &values, const Texture &lineLengths, bool darkOnLight)
{
    write->Use();
    // todo: need not pass x,y gradients, only direction matters
    write->Uniforms["Gradients"].SetValue(*gradients);
    write->Uniforms["LineLengths"].SetValue(lineLengths);
    write->Uniforms["Values"].SetValue(values);
    write->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto result = Render(PrimitiveType::Lines);
    RenderWindow::Instance().AddTexture(result, "SWT values");
    return result;
}

Ptr<Texture> SWTFilter::AverageRayValues(const Texture &values, bool darkOnLight)
{
    avg->Use();
    avg->Uniforms["Gradients"].SetValue(*gradients);
    avg->Uniforms["LineLengths"].SetValue(values);
    avg->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    auto result = Render();
    RenderWindow::Instance().AddTexture(result, "Avg SWT values");
    return result;
}

void SWTFilter::PrepareEdgeOnlyStencil()
{
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void SWTFilter::PrepareMaximizingDepthTest()
{
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
}

void SWTFilter::PrepareRayLines(const Texture &input)
{
    int width  = input.GetWidth();
    int height = input.GetHeight();
    
    List<VertexPositionTexture> vertices(1024);
    GLfloat buffer[width * height];
    input.GetTextureImage(GL_RED, GL_FLOAT, buffer);
    
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
    {
        if (buffer[i + j * width] == 0.0f)
            continue;
        
        VertexPositionTexture v1, v2;
        v1.Position = Vector3(i, j, 0); // z == 0 = Use directly
        v2.Position = Vector3(i, j, 1); // z == 1 = Scatter position to end point
        vertices.push_back(v1);
        vertices.push_back(v2);
    }
    
    linesVertices = New<VertexBuffer>();
    linesVertices->SetData(vertices);
}

Ptr<Texture> SWTFilter::ScaleResult(const Texture &input, float scaleFactor)
{
    scale->Use();
    scale->Uniforms["Texture"].SetValue(input);
    scale->Uniforms["Scale"].SetValue(scaleFactor);
    return Render();
}