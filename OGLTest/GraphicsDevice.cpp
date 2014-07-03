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

Ptr<::VertexBuffer> GraphicsDevice::VertexBuffer = nullptr;
Ptr<::IndexBuffer>  GraphicsDevice::IndexBuffer  = nullptr;

Ptr<::VertexBuffer> GraphicsDevice::DefaultVertexBuffer = nullptr;
Ptr<::IndexBuffer>  GraphicsDevice::DefaultIndexBuffer  = nullptr;

void GraphicsDevice::DrawPrimitives()
{
    // bind the index buffer
    IndexBuffer->Bind();
    // draw the VAO
    // todo: kan miss een kleiner datatype dan int, omdat er voor per pixel geen indices gebruikt worden
    glDrawElements((GLenum)(IndexBuffer->PrimitiveType), IndexBuffer->Count(), GL_UNSIGNED_INT, (GLvoid*)NULL);
}

void GraphicsDevice::DrawArrays(PrimitiveType primitiveType)
{
    // Draw without index buffer
    glDrawArrays((GLenum)primitiveType, 0, VertexBuffer->Count());
}