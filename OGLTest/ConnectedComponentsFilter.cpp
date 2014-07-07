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
#include "VertexPosition.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "RenderWindow.h"
#include "SWTParameters.h"
#include "BoundingBox.h"

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
    
    // todo: for debug purposes
    normal         = LoadScreenSpaceProgram("Normal");
    decode         = LoadScreenSpaceProgram("Decode");
}

void ConnectedComponentsFilter::Initialize()
{
    //PrepareMaximizingDepthTest();
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
    
    List<VertexPosition> vertices;
    for (int y = 0; y < height; ++y)
        vertices.push_back(VertexPosition(Vector3(0, y, 0)));
    
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
    ReserveColorBuffers(2/*1*/);
    
    int width = Input->GetWidth();
    
    auto tex1  = ColorBuffers[0];
    auto tex2  = output;
    auto debug = ColorBuffers[1];
    
    // Compute vertical runs
    Encode(Input, tex2);
    
    // todo: for debug purposes
    Decode(tex2, debug); DEBUG_FB("Encoded");

    VerticalRuns(tex2, tex1);
    
    PrepareVerticalRuns();
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // hmm nu die swaps goed zijn, was die copy misschien niet eens nodig en kan het misschien nog steeds zonder min/max
    // Column processing
    for(int column = width - 2; column >= 0; --column)
    {
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        GatherNeighbor(tex1, column, tex2);
        
        GraphicsDevice::SetBuffers(columnVertices, lineIndices);
        UpdateColumn(tex2, column, tex1);
        
        GraphicsDevice::SetBuffers(columnVertices, nullptr);
        glDepthMask(GL_TRUE);
        ScatterBack(tex1, column, tex2);
        glDepthMask(GL_FALSE);
    }
    
    //FrameBuffer::GetCurrentlyBound()->Print(RenderBufferType::Depth);
    
    GraphicsDevice::UseDefaultBuffers();
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST); // should enable, but gives incorrect results
    
    // Post column processing
    glDepthFunc(GL_EQUAL);
    UpdateRoots(tex2, tex1);
    glDisable(GL_DEPTH_TEST);
    
    UpdateChildren(tex1, tex2);

    Decode(tex2, debug); DEBUG_FB("Components");
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
    verticalRun->Uniforms["Encoded"].SetValue(*input);
    verticalRun->Uniforms["StrokeWidths"].SetValue(*Input);
    verticalRun->Uniforms["PassIndex"].SetValue(0);
    RenderToTexture(output);
    
    int log_h = (int)log2f(height);
    for(int i = 1; i <= log_h; ++i)
    {
        std::swap(input, output);
        verticalRun->Uniforms["Encoded"].SetValue(*input);
        verticalRun->Uniforms["StrokeWidths"].SetValue(*Input);
        verticalRun->Uniforms["PassIndex"].SetValue(i);
        RenderToTexture(output);
    }
}

void ConnectedComponentsFilter::GatherNeighbor(Ptr<Texture> input, int column, Ptr<Texture> output)
{
    gatherNeighbor->Use();
    gatherNeighbor->Uniforms["Texture"].SetValue(*input);
    gatherNeighbor->Uniforms["StrokeWidths"].SetValue(*Input);
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
    scatterBack->Uniforms["StrokeWidths"].SetValue(*Input);
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

void ConnectedComponentsFilter::PrepareVerticalRuns()
{
    glBlendEquation(GL_MAX);
    glDepthFunc(GL_ALWAYS);
    glClearDepth(0);
    glDepthMask(GL_FALSE);
}

void ConnectedComponentsFilter::Decode(Ptr<Texture> input, Ptr<Texture> output)
{
    decode->Use();
    decode->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output);
}