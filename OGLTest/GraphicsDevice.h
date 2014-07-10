//
//  GraphicsDevice.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "PrimitiveType.h"

class GraphicsDevice
{
public:
    
    static void DrawPrimitives();
    
    static void DrawArrays(PrimitiveType primitiveType);
    
    static void SetBuffers(Ptr<::VertexBuffer> vertexBuffer, Ptr<::IndexBuffer> indexBuffer);
    
    static void SetDefaultBuffers(Ptr<::VertexBuffer> vertexBuffer, Ptr<::IndexBuffer> indexBuffer);
    
    static void UseDefaultBuffers();
    
    static void AssertCompleteProgram();
    
public:
    
    static Ptr<::VertexBuffer> VertexBuffer;
    static Ptr<::IndexBuffer>  IndexBuffer;
    
    static Ptr<::VertexBuffer> DefaultVertexBuffer;
    static Ptr<::IndexBuffer>  DefaultIndexBuffer;
};

inline void GraphicsDevice::SetBuffers(Ptr<::VertexBuffer> vertexBuffer, Ptr<::IndexBuffer> indexBuffer)
{
    VertexBuffer = vertexBuffer;
    IndexBuffer  = indexBuffer;
}

inline void GraphicsDevice::SetDefaultBuffers(Ptr<::VertexBuffer> vertexBuffer, Ptr<::IndexBuffer> indexBuffer)
{
    DefaultVertexBuffer = vertexBuffer;
    DefaultIndexBuffer  = indexBuffer;
}

inline void GraphicsDevice::UseDefaultBuffers()
{
    VertexBuffer = DefaultVertexBuffer;
    IndexBuffer  = DefaultIndexBuffer;
}