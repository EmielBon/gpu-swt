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
    
    void DrawPrimitives(PrimitiveType type);
    
public:
    
    Ptr<VertexBuffer> VertexBuffer;
    Ptr<VertexArray>  VertexArray;
    Ptr<IndexBuffer>  IndexBuffer;
};
