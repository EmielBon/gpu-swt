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

void ConnectedComponentsFilter::Copy(Ptr<Texture> texture, Ptr<Texture> output)
{
    GraphicsDevice::UseDefaultBuffers();
    
    normal->Use();
    normal->Uniforms["Texture"].SetValue(*texture);
    RenderToTexture(output);
}

void ConnectedComponentsFilter::LoadShaderPrograms()
{
    encode         = LoadScreenSpaceProgram("Encode");
    verticalRun    = LoadScreenSpaceProgram("VerticalRuns");
    gatherNeighbor = LoadProgram("GatherNeighbor", "GatherScatter");
    updateColumn   = LoadProgram("UpdateColumn");
    scatterBack    = LoadProgram("ScatterBack", "GatherScatter");
    updateRoots    = LoadScreenSpaceProgram("UpdateRoots");
    updateChildren = LoadScreenSpaceProgram("UpdateChildren");
    normal         = LoadScreenSpaceProgram("Normal");
}

void ConnectedComponentsFilter::Initialize()
{
    PrepareMaximizingDepthTest();
    PrepareColumnVertices();
    PrepareLineIndices();
    glClearColor(0, 0, 0, 0);
}

void ConnectedComponentsFilter::PrepareMaximizingDepthTest()
{
    glClearDepth(0);
    glDepthFunc(GL_GREATER);
}

void ConnectedComponentsFilter::PrepareColumnVertices()
{
    int height = Input->GetHeight();
    
    List<VertexPositionTexture> vertices;
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
    ReserveColorBuffers(1);
    
    int width = Input->GetWidth();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    Encode(Input, ColorBuffers[0]);
    DEBUG_FB("Encoded positions");
    VerticalRuns(ColorBuffers[0], output);
    DEBUG_FB("Vertical runs");
    
    auto tex1 = output;
    auto tex2 = ColorBuffers[0];
    
    for(int column = width - 2; column >= 0; --column)
    {
        glEnable(GL_DEPTH_TEST);
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        GatherNeighbor(tex1, column, tex2);
        glDisable(GL_DEPTH_TEST);
        
        Copy(tex2, tex1);
        
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        UpdateColumn(tex1, column, tex2);
        
        Copy(tex2, tex1);
        
        glEnable(GL_DEPTH_TEST);
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        ScatterBack(tex1, column, tex2);
        glDisable(GL_DEPTH_TEST);
        
        Copy(tex2, tex1);
    }
    
    DEBUG_FB("Myeah");
    
    GraphicsDevice::UseDefaultBuffers();
    
    DEBUG_FB("Column processing result");
    
    UpdateRoots(tex1, tex2);
    
    DEBUG_FB("Update roots");
    
    UpdateChildren(tex2, tex1);
    
    //DEBUG_FB("Update children");
}

void ConnectedComponentsFilter::Encode(Ptr<Texture> input, Ptr<Texture> output)
{
    encode->Use();
    encode->Uniforms["Texture"].SetValue(*input);
    encode->Uniforms["BackgroundColor"].SetValue( Vector3(0, 0, 0) );
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

void ConnectedComponentsFilter::UpdateColumn(Ptr<Texture> input, int column, Ptr<Texture> output)
{
    updateColumn->Use();
    updateColumn->Uniforms["Texture"].SetValue(*input);
    updateColumn->Uniforms["Column"].SetValue(column);
    RenderToTexture(output, PrimitiveType::Points);
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