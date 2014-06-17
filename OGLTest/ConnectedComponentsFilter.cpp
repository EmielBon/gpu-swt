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
    boundingBoxes  = LoadProgram("BoundingBoxes");
    filterInvalidComponents = LoadScreenSpaceProgram("FilterInvalidComponents");
    countComponents = LoadProgram("CountComponents");
    
    // todo: for debug purposes
    normal         = LoadScreenSpaceProgram("Normal");
    decode         = LoadScreenSpaceProgram("Decode");
}

void ConnectedComponentsFilter::Initialize()
{
    //PrepareMaximizingDepthTest();
    PrepareColumnVertices();
    PrepareLineIndices();
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);
    
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

    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Compute vertical runs
    Encode(Input, ColorBuffers[0]);
    Decode(ColorBuffers[0], ColorBuffers[1]);
    DEBUG_FB("lala");
    VerticalRuns(ColorBuffers[0], output);
    
    auto tex1 = output;
    auto tex2 = ColorBuffers[0];
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    
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
    
    GraphicsDevice::UseDefaultBuffers();
    
    glDisable(GL_BLEND);
    
    // Post column processing
    glDepthFunc(GL_EQUAL);
    UpdateRoots(tex2, tex1);
    glDisable(GL_DEPTH_TEST);
    
    UpdateChildren(tex1, tex2);

    Decode(tex2, ColorBuffers[1]);
    DEBUG_FB("Components");
    
    // Compute bounding boxes
    int height = Input->GetHeight();
    
    List<VertexPosition> vertices;
    for (int x = 0; x < width;  ++x)
    for (int y = 0; y < height; ++y)
        vertices.push_back(VertexPosition(Vector3(x, y, 0)));
    
    auto pixelVertices = New<VertexBuffer>();
    pixelVertices->SetData(vertices);
    
    GraphicsDevice::SetBuffers(pixelVertices, nullptr);
    
    glEnable(GL_BLEND);
    
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    BoundingBoxes(tex2, tex1);
    glDisable(GL_BLEND);
    
    DEBUG_FB("BBoxes");
    
    GraphicsDevice::UseDefaultBuffers();
    
    FilterInvalidComponents(tex1, tex2);
    DEBUG_FB("Invalids");
    
    cv::Vec4f pixels[800 * 600];
    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_FLOAT, pixels);
    for(auto& pixel : pixels)
    {
        int x1 = -((int)pixel[0] - 799);
        int y1 = -((int)pixel[1] - 599);
        int x2 = (int)pixel[2];
        int y2 = (int)pixel[3];
        if (x1 != x2 && y1 != y2 && x2 != 0 && y2 != 0)
            ExtractedBoundingBoxes.push_back(BoundingBox(cv::Rect(x1, y1, x2 - x1, y2 - y1)));
    }
    
    GraphicsDevice::SetBuffers(pixelVertices, nullptr);
    
    // Count unique components
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    CountComponents(tex2, tex1);
    glDisable(GL_BLEND);
    
    // Zou (1, 1) moeten zijn, maar er gaat misschien toch iets mis met getScreenTexCoord
    cv::Vec4f pixel = FrameBuffer::ReadPixel(1, 1);
    
    printf("%i\n", (int)pixel[0]);
    
    GraphicsDevice::UseDefaultBuffers();
    
    // Stencil routing
    
    // Retrieve results
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

void ConnectedComponentsFilter::BoundingBoxes(Ptr<Texture> input, Ptr<Texture> output)
{
    boundingBoxes->Use();
    boundingBoxes->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
}

void ConnectedComponentsFilter::FilterInvalidComponents(Ptr<Texture> input, Ptr<Texture> output)
{
    filterInvalidComponents->Use();
    filterInvalidComponents->Uniforms["Texture"].SetValue(*input);
    filterInvalidComponents->Uniforms["MinAspectRatio"].SetValue(MinAspectRatio);
    filterInvalidComponents->Uniforms["MaxAspectRatio"].SetValue(MaxAspectRatio);
    filterInvalidComponents->Uniforms["MinSizeRatio"].SetValue(0.0005f);
    filterInvalidComponents->Uniforms["MaxSizeRatio"].SetValue(0.02f);
    RenderToTexture(output);
}

void ConnectedComponentsFilter::CountComponents(Ptr<Texture> input, Ptr<Texture> output)
{
    countComponents->Use();
    countComponents->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
}

void ConnectedComponentsFilter::Decode(Ptr<Texture> input, Ptr<Texture> output)
{
    decode->Use();
    decode->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output);
}