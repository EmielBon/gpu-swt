//
//  DrawableRect.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "VertexPositionTexture.h"

class DrawableRect
{
public:
    
    DrawableRect(float x1, float y1, float x2, float y2);
    
public:
    
    Ptr<VertexBuffer> VertexBuffer;
    Ptr<IndexBuffer> IndexBuffer;
};