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
    
    static void DrawPrimitives(PrimitiveType type);
    
    static void SetBuffers(Ptr<VertexBuffer> vertexBuffer, Ptr<IndexBuffer> indexBuffer);
    
public:
    
    static Ptr<VertexBuffer> VertexBuffer;
    static Ptr<IndexBuffer>  IndexBuffer;
};

inline void GraphicsDevice::SetBuffers(Ptr<::VertexBuffer> vertexBuffer, Ptr<::IndexBuffer> indexBuffer)
{
    VertexBuffer = vertexBuffer;
    IndexBuffer  = indexBuffer;
}