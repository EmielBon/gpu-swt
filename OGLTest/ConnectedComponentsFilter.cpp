//
//  ConnectedComponentsFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 28-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "ConnectedComponentsFilter.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "VertexPositionTexture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "RenderWindow.h"

#define DEBUG_FB(name) RenderWindow::Instance().AddFrameBufferSnapshot(name)

void ConnectedComponentsFilter::LoadShaderPrograms()
{
    encode         = LoadScreenSpaceProgram("Encode");
    verticalRun    = LoadScreenSpaceProgram("VerticalRuns");
    gatherNeighbor = LoadProgram("GatherNeighbor", "GatherScatter");
    updateColumn   = LoadScreenSpaceProgram("UpdateColumn");
    scatterBack    = LoadProgram("ScatterBack", "GatherScatter");
    updateRoots    = LoadScreenSpaceProgram("UpdateRoots");
    updateChildren = LoadScreenSpaceProgram("UpdateChildren");
}

void ConnectedComponentsFilter::Initialize()
{
    PrepareMaximizingDepthTest();
    PrepareColumnVertices();
    PrepareLineIndices();
}

void ConnectedComponentsFilter::PrepareMaximizingDepthTest()
{
    glClearDepth(0);
    glDepthFunc(GL_GREATER);
}

void ConnectedComponentsFilter::PrepareColumnVertices()
{
    int height = Input->GetHeight();
    
    List<VertexPositionTexture> vertices(height);
    for (int y = 0; y < height; ++y)
        vertices.push_back(VertexPositionTexture(Vector3(0, y, 0), Vector2(0, 0)));
    
    columnVertices = New<VertexBuffer>();
    columnVertices->SetData(vertices);
}

void ConnectedComponentsFilter::PrepareLineIndices()
{
    int height = Input->GetHeight();
    
    List<GLuint> indices = { 0, (GLuint)height - 1 };
    
    lineIndices = New<IndexBuffer>();
    lineIndices->SetData(indices);
}

void ConnectedComponentsFilter::PerformSteps(Ptr<Texture> output)
{
    int width  = Input->GetWidth();
    //int height = Input->GetHeight();
    
    ReserveColorBuffers(1);
    
    Encode(Input, output); // Output contains encoded positions
    DEBUG_FB("Encoded positions");
    VerticalRuns(output, ColorBuffers[0]); // ColorBuffer[0] contains vertical runs
    DEBUG_FB("Vertical runs");
    
    auto input = ColorBuffers[0];
    std::swap(input, output);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    for(int column = width - 2; column >= 0; --column)
    {
        std::swap(input, output);
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        glEnable(GL_DEPTH_TEST);
        GatherNeighbor(input, column, output);
        glDisable(GL_DEPTH_TEST);
        
        std::swap(input, output);
        GraphicsDevice::SetBuffers(columnVertices, lineIndices);
        UpdateColumn(input, output);
        
        std::swap(input, output);
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        glEnable(GL_DEPTH_TEST);
        ScatterBack(input, column, output);
        glDisable(GL_DEPTH_TEST);
    }
    
    DEBUG_FB("Column processing result");
    
    GraphicsDevice::UseDefaultBuffers();
    std::swap(input, output);
    UpdateRoots(input, output);
    
    DEBUG_FB("Update roots");
    
    /*std::swap(input, output);
    UpdateChildren(input, output);
    
    DEBUG_FB("Update children");*/
}

void ConnectedComponentsFilter::Encode(Ptr<Texture> input, Ptr<Texture> output)
{
    encode->Use();
    encode->Uniforms["Texture"].SetValue(*input);
    encode->Uniforms["BackgroundColor"].SetValue(0.0f);
    RenderToTexture(output);
}

void ConnectedComponentsFilter::VerticalRuns(Ptr<Texture> input, Ptr<Texture> output)
{
    int height = input->GetHeight();
    
    verticalRun->Use();
    verticalRun->Uniforms["Texture"].SetValue(*input);
    verticalRun->Uniforms["PassIndex"].SetValue(0);
    RenderToTexture(output);
    
    int log_h = (int)log2f(height);
    for(int i = 1; i <= log_h; ++i)
    {
        std::swap(input, output);
        verticalRun->Uniforms["Texture"].SetValue(*input);
        verticalRun->Uniforms["PassIndex"].SetValue(i);
        RenderToTexture(output);
    }
}

void ConnectedComponentsFilter::GatherNeighbor(Ptr<Texture> input, int column, Ptr<Texture> output)
{
    gatherNeighbor->Use();
    gatherNeighbor->Uniforms["Texture"].SetValue(*input);
    gatherNeighbor->Uniforms["Column"].SetValue(column);
    RenderToTexture(output, PrimitiveType::Points);
}

void ConnectedComponentsFilter::UpdateColumn(Ptr<Texture> input, Ptr<Texture> output)
{
    updateColumn->Use();
    updateColumn->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Lines);
}

void ConnectedComponentsFilter::ScatterBack(Ptr<Texture> input, int column, Ptr<Texture> output)
{
    scatterBack->Use();
    scatterBack->Uniforms["Texture"].SetValue(*input);
    scatterBack->Uniforms["Column"].SetValue(column);
    RenderToTexture(output, PrimitiveType::Points);
}

void ConnectedComponentsFilter::UpdateRoots(Ptr<Texture> input, Ptr<Texture> output)
{
    updateRoots->Use();
    updateRoots->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT);
}

void ConnectedComponentsFilter::UpdateChildren(Ptr<Texture> input, Ptr<Texture> output)
{
    updateChildren->Use();
    updateChildren->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT);
}