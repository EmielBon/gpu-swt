//
//  GraphicsDevice.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GraphicsDevice.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Program.h"

Ptr<::VertexBuffer> GraphicsDevice::VertexBuffer = nullptr;
Ptr<::IndexBuffer>  GraphicsDevice::IndexBuffer  = nullptr;

Ptr<::VertexBuffer> GraphicsDevice::DefaultVertexBuffer = nullptr;
Ptr<::IndexBuffer>  GraphicsDevice::DefaultIndexBuffer  = nullptr;

void GraphicsDevice::DrawPrimitives()
{
    AssertCompleteProgram();
    // bind the index buffer
    IndexBuffer->Bind();
    // draw the VAO
    // todo: kan miss een kleiner datatype dan int, omdat er voor per pixel geen indices gebruikt worden
    glDrawElements((GLenum)(IndexBuffer->PrimitiveType), IndexBuffer->Count(), GL_UNSIGNED_INT, (GLvoid*)NULL);
}

void GraphicsDevice::DrawArrays(PrimitiveType primitiveType)
{
    AssertCompleteProgram();
    // Draw without index buffer
    glDrawArrays((GLenum)primitiveType, 0, VertexBuffer->Count());
}

void GraphicsDevice::AssertCompleteProgram()
{
    if (Program::GetCurrentlyUsed() == nullptr)
        throw std::runtime_error("Error: Rendering without program");
        
    for (auto& keyValues : Program::GetCurrentlyUsed()->Uniforms)
    {
        auto &uniform = keyValues.second;
        if (!uniform.IsInitialized())
            throw std::runtime_error("Error: No value specified for uniform \"" + uniform.GetName() + "\"");
    }
}