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
#include "FrameBuffer.h"

void ConnectedComponentsFilter::LoadShaderPrograms()
{
    encode         = LoadScreenSpaceProgram("Encode");
    verticalRun    = LoadScreenSpaceProgram("VerticalRuns");
    gatherNeighbor = LoadProgram("GatherNeighbor", "GatherScatter");
    updateColumn   = LoadScreenSpaceProgram("UpdateColumn");
    scatterBack    = LoadProgram("ScatterBack", "GatherScatter");
}

void ConnectedComponentsFilter::Initialize()
{
    PrepareMaximizingDepthTest();
}

void ConnectedComponentsFilter::PrepareMaximizingDepthTest()
{
    glClearDepth(0);
    glDepthFunc(GL_GREATER);
}

Ptr<Texture> ConnectedComponentsFilter::Encode()
{
    encode->Use();
    encode->Uniforms["Texture"].SetValue(*Input);
    encode->Uniforms["BackgroundColor"].SetValue(0.0f);
    return Render();
}

Ptr<Texture> ConnectedComponentsFilter::VerticalRuns(Ptr<Texture> input)
{
    int height = input->GetHeight();
    auto tex0  = input->GetEmptyClone();
    
    verticalRun->Use();
    
    verticalRun->Uniforms["Texture"].SetValue(*input);
    verticalRun->Uniforms["PassIndex"].SetValue(0);
    auto tex1 = Render();
    
    int log_h = (int)log2f(height);
    for(int i = 1; i <= log_h; ++i)
    {
        std::swap(tex0, tex1);
        verticalRun->Uniforms["Texture"].SetValue(*tex0);
        verticalRun->Uniforms["PassIndex"].SetValue(i);
        RenderToTexture(tex1);
    }
    return tex1;
}

void ConnectedComponentsFilter::GatherNeighbor(Ptr<Texture> input, Ptr<Texture> dest)
{
    gatherNeighbor->Use();
    gatherNeighbor->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(dest, PrimitiveType::Points);
}

void ConnectedComponentsFilter::UpdateColumn(Ptr<Texture> input, Ptr<Texture> dest)
{
    updateColumn->Use();
    updateColumn->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(dest);
}

void ConnectedComponentsFilter::ScatterBack(Ptr<Texture> input, Ptr<Texture> dest)
{
    scatterBack->Use();
    scatterBack->Uniforms["Texture"].SetValue(*input);
    RenderToTexture(dest, PrimitiveType::Points);
}

Ptr<Texture> ConnectedComponentsFilter::PerformSteps()
{
    auto quadVertices = GraphicsDevice::VertexBuffer;
    auto quadIndices  = GraphicsDevice::IndexBuffer;
    
    int width  = Input->GetWidth();
    int height = Input->GetHeight();
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    auto encodedPositions = Encode();
    auto verticalRuns     = VerticalRuns(encodedPositions);
    
    GLfloat buffer[width * height];
    verticalRuns->GetTextureImage(GL_BLUE, GL_FLOAT, buffer);
    
    auto input  = verticalRuns;
    auto output = input->GetEmptyClone();
    
    List<VertexPositionTexture> vertices(height);
    
    for(int column = width - 2; column >= 0; --column)
    {
        vertices.clear();
        
        for (int y = 0; y < height; ++y)
        {
            float rootID = buffer[column + y * width];
            if (rootID != 0.0)
                vertices.push_back(VertexPositionTexture(Vector3(column, y, rootID), Vector2(0, 0)));
        }
        
        Ptr<VertexBuffer> pixelVertices = New<VertexBuffer>();
        pixelVertices->SetData(vertices);
        
        GraphicsDevice::SetBuffers(pixelVertices, nullptr);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        GatherNeighbor(input, output);
        std::swap(input, output);
        glDisable(GL_DEPTH_TEST);
        GraphicsDevice::SetBuffers(quadVertices, quadIndices);
        
        UpdateColumn(input, output);
        std::swap(input, output);
        
        GraphicsDevice::SetBuffers(pixelVertices, nullptr);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        ScatterBack(input, output);
        std::swap(input, output);
        glDisable(GL_DEPTH_TEST);
        GraphicsDevice::SetBuffers(quadVertices, quadIndices);
    }
    
    return input;
}