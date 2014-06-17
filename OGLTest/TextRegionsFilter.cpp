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

void TextRegionsFilter::LoadShaderPrograms()
{
    grayFilter = New<GrayFilter>(Input);
    swtFilter  = New<SWTFilter>();
    connectedComponentsFilter = New<ConnectedComponentsFilter>();
    
    grayFilter->DoLoadShaderPrograms();
    swtFilter->DoLoadShaderPrograms();
    connectedComponentsFilter->DoLoadShaderPrograms();
    
    //vertexTexture = LoadProgram("VertexTexture");
}

void TextRegionsFilter::Initialize()
{
    gray = GetColorAttachment()->GetEmptyClone();
    ApplyFilter(*grayFilter, gray);
    swtFilter->Input = gray;
    
    ReserveColorBuffers(1);
}

void TextRegionsFilter::PerformSteps(Ptr<Texture> output)
{
    /*int width  = Input->GetWidth();
    int height = Input->GetHeight();
    
    List<VertexPosition> vertices;
    for (int x = 0; x < width;  ++x)
        for (int y = 0; y < height; ++y)
            vertices.push_back(VertexPosition(Vector3(x, y, 0)));
    
    auto pixelVertices = New<VertexBuffer>();
    pixelVertices->SetData(vertices);
    
    GraphicsDevice::SetBuffers(pixelVertices, nullptr);
    
    vertexTexture->Use();
    vertexTexture->Uniforms["Texture"].SetValue(*Input);
    RenderToTexture(output, PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
    
    GraphicsDevice::UseDefaultBuffers();*/
    
    swtFilter->GradientDirection = GradientDirection;
    ApplyFilter(*swtFilter, ColorBuffers[0]);
    swtFilter->GradientDirection = GradientDirection::Against;
    connectedComponentsFilter->Input = ColorBuffers[0];
    ApplyFilter(*connectedComponentsFilter, output);
}

List<BoundingBox> TextRegionsFilter::GetExtractedBoundingBoxes() const
{
    return connectedComponentsFilter->ExtractedBoundingBoxes;
}