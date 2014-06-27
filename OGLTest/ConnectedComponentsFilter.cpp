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
    PreparePerPixelVertices();
    
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

void ConnectedComponentsFilter::PrepareBoundingBoxCalculation()
{
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
}
void ConnectedComponentsFilter::PrepareComponentCounting()
{
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

void ConnectedComponentsFilter::PreparePerPixelVertices()
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

void ConnectedComponentsFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(2/*1*/);
    
    int width = Input->GetWidth();
    
    // Compute vertical runs
    Encode(Input, ColorBuffers[0]);
    Decode(ColorBuffers[0], ColorBuffers[1]);
    DEBUG_FB("lala");
    VerticalRuns(ColorBuffers[0], output);
    
    auto tex1 = output;
    auto tex2 = ColorBuffers[0];
    
    PrepareVerticalRuns();
    
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
    glDisable(GL_DEPTH_TEST); // should enable, but gives incorrect results
    // Post column processing
    glDepthFunc(GL_EQUAL);
    UpdateRoots(tex2, tex1);
    glDisable(GL_DEPTH_TEST);
    
    UpdateChildren(tex1, tex2);

    Decode(tex2, ColorBuffers[1]);
    DEBUG_FB("Components");
    
    // Compute bounding boxes
    PrepareBoundingBoxCalculation();
    GraphicsDevice::SetBuffers(perPixelVertices, nullptr);
    glEnable(GL_BLEND);
    BoundingBoxes(tex2, tex1);
    glDisable(GL_BLEND);
    DEBUG_FB("BBoxes");
    
    GraphicsDevice::UseDefaultBuffers();
    
    FilterInvalidComponents(tex1, tex2);
    ExtractBoundingBoxes();
    DEBUG_FB("Invalids");
    
    // Count unique components
    GraphicsDevice::SetBuffers(perPixelVertices, nullptr);
    PrepareComponentCounting();
    glEnable(GL_BLEND);
    CountComponents(tex2, ColorBuffers[1]);
    glDisable(GL_BLEND);
    cv::Vec4f pixel = FrameBuffer::ReadPixel(1, 1);
    printf("Number of unique components: %i\n", (int)pixel[0]);
    
    GraphicsDevice::UseDefaultBuffers();
    
    // Stencil routing
    //glEnable(GL_STENCIL_TEST);
    //glClear(GL_STENCIL_BUFFER_BIT);
    
    
    
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

void ConnectedComponentsFilter::PrepareVerticalRuns()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glClearDepth(0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
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

void ConnectedComponentsFilter::ExtractBoundingBoxes()
{
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
}

void ConnectedComponentsFilter::Decode(Ptr<Texture> input, Ptr<Texture> output)
{
    decode->Use();
    decode->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(output);
}