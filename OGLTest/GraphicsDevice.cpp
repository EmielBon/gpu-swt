//
//  GraphicsDevice.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GraphicsDevice.h"
#include "IndexBuffer.h"

Ptr<VertexBuffer> GraphicsDevice::VertexBuffer = nullptr;
Ptr<IndexBuffer>  GraphicsDevice::IndexBuffer  = nullptr;

void GraphicsDevice::DrawPrimitives()
{
    // bind the index buffer
    IndexBuffer->Bind();
    // draw the VAO
    glDrawElements((GLenum)(IndexBuffer->PrimitiveType), IndexBuffer->Count(), GL_UNSIGNED_INT, (GLvoid*)NULL);
}