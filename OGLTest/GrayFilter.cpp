//
//  GrayFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GrayFilter.h"
#include "VertexPosition.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "GraphicsDevice.h"

void GrayFilter::PerformSteps(Ptr<Texture> output)
{
    /*List<VertexPosition> vertices(2);
    vertices[0].Position = Vector3(10, 10, 0);
    vertices[1].Position = Vector3(500, 400, 0);
    auto vb = New<VertexBuffer>();
    vb->SetData(vertices);
    GraphicsDevice::SetBuffers(vb, nullptr);
    
    auto program = LoadProgram("VertexTexture");
    program->Use();
    program->Uniforms["Texture"].SetValue(*Input);
    program->Uniforms["Color"].SetValue(cv::Vec4f(1, 1, 1, 1));
    
    glClearColor(0, 0, 0, 1);
    RenderToTexture(output, PrimitiveType::Lines, GL_COLOR_BUFFER_BIT);
    GraphicsDevice::UseDefaultBuffers();*/
    
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(*Input);
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT);
}