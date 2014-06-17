//
//  SWTFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTFilter.h"
#include "VertexPosition.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "SobelFilter.h"
#include "CannyFilter.h"
#include "RenderWindow.h"
#include "GaussianFilter.h"
#include "FrameBuffer.h"

void SWTFilter::LoadShaderPrograms()
{
    sobel    = New<SobelFilter>();
    gaussian = New<GaussianFilter>();
    canny    = New<CannyFilter>();
    
    sobel->DoLoadShaderPrograms();
    gaussian->DoLoadShaderPrograms();
    canny->DoLoadShaderPrograms();
    
    cast     = LoadScreenSpaceProgram("CastRays");
    write    =            LoadProgram("WriteRays");
    avg      = LoadScreenSpaceProgram("AverageRays");
    writeAvg = LoadProgram("WriteAverageRays", "WriteRays");
    scale    = LoadScreenSpaceProgram("ScaleColor");
}

void SWTFilter::Initialize()
{
    ReserveColorBuffers(1);
    
    gradients = Input->GetEmptyClone();
    edges     = Input->GetEmptyClone();
    
    sobel->Input    = Input;
    gaussian->Input = Input;
    
    ApplyFilter(*sobel, gradients);
    ApplyFilter(*gaussian, ColorBuffers[0]);
    
    canny->Input = ColorBuffers[0];
    ApplyFilter(*canny, edges); // Builds an edge-only stencil buffer
    
    PrepareEdgeOnlyStencil();
    //PrepareMaximizingDepthTest();
    PrepareRayLines(*edges);
    
    glClearColor(0, 0, 0, 0);
}

void SWTFilter::PrepareEdgeOnlyStencil()
{
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

/*void SWTFilter::PrepareMaximizingDepthTest()
{
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
}*/

void SWTFilter::PrepareRayLines(const Texture &input)
{
    int width  = input.GetWidth();
    int height = input.GetHeight();
    
    GLfloat buffer[width * height];
    input.GetTextureImage(GL_RED, GL_FLOAT, buffer);
    
    int count = 0;
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
    {
        if (buffer[i + j * width] != 0.0f)
            count++;
    }
    
    List<VertexPosition> vertices;
    vertices.reserve(count);
    
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
    {
        if (buffer[i + j * width] == 0.0f)
            continue;
        
        VertexPosition v1, v2;
        v1.Position = Vector3(i, j, 0); // z == 0 = Use directly
        v2.Position = Vector3(i, j, 1); // z == 1 = Scatter position to end point
        vertices.push_back(v1);
        vertices.push_back(v2);
    }
    
    linesVertices = New<VertexBuffer>();
    linesVertices->SetData(vertices);
}

void SWTFilter::PerformSteps(Ptr<Texture> output)
{
    if (GradientDirection == GradientDirection::Unspecified)
        throw std::runtime_error("Unspecified gradient direction in SWT step");
    
    bool darkOnLight = (GradientDirection == GradientDirection::With);
    
    ReserveColorBuffers(2);
    
    auto swt               = output;
    auto oppositePositions = ColorBuffers[0];
    auto averageValues     = ColorBuffers[1];
    
    glEnable(GL_STENCIL_TEST);
    PrepareEdgeOnlyStencil();
    CastRays(darkOnLight, oppositePositions);
    glDisable(GL_STENCIL_TEST);
    DEBUG_FB("SWT 1");
    GraphicsDevice::SetBuffers(linesVertices, nullptr);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_MAX);
    WriteRayValues(*oppositePositions, swt);
    glDisable(GL_BLEND);
    GraphicsDevice::UseDefaultBuffers();
    DEBUG_FB("SWT 2");
    glEnable(GL_STENCIL_TEST);
    AverageRayValues(*oppositePositions, *swt, averageValues);
    glDisable(GL_STENCIL_TEST);
    DEBUG_FB("SWT 3");
    GraphicsDevice::SetBuffers(linesVertices, nullptr);
    glEnable(GL_BLEND);
    WriteAverageRayValues(*oppositePositions, *averageValues, output);
    glDisable(GL_BLEND);
    DEBUG_FB("SWT 4");
    
    GraphicsDevice::UseDefaultBuffers();
}

void SWTFilter::CastRays(bool darkOnLight, Ptr<Texture> output)
{
    cast->Use();
    cast->Uniforms["Edges"].SetValue(*edges);
    cast->Uniforms["Gradients"].SetValue(*gradients);
    cast->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT);
}

void SWTFilter::WriteRayValues(const Texture &oppositePositions, Ptr<Texture> output)
{
    write->Use();
    write->Uniforms["OppositePositions"].SetValue(oppositePositions);
    RenderToTexture(output, PrimitiveType::Lines, GL_COLOR_BUFFER_BIT);
}

void SWTFilter::AverageRayValues(const Texture &oppositePositions, const Texture &values, Ptr<Texture> output)
{
    avg->Use();
    avg->Uniforms["OppositePositions"].SetValue(oppositePositions);
    avg->Uniforms["Values"].SetValue(values);
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT);
}

void SWTFilter::WriteAverageRayValues(const Texture &oppositePositions, const Texture &averageValues, Ptr<Texture> output)
{
    writeAvg->Use();
    writeAvg->Uniforms["OppositePositions"].SetValue(oppositePositions);
    writeAvg->Uniforms["AverageValues"].SetValue(averageValues);
    RenderToTexture(output, PrimitiveType::Lines, GL_COLOR_BUFFER_BIT);
}

void SWTFilter::ScaleResult(const Texture &input, float scaleFactor, Ptr<Texture> output)
{
    scale->Use();
    scale->Uniforms["Texture"].SetValue(input);
    scale->Uniforms["Scale"].SetValue(scaleFactor);
    RenderToTexture(output);
}