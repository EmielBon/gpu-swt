//
//  DepthBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "types.h"

class RenderBuffer : public IOGLBindableResource<RenderBuffer>
{
public:
    
    enum class Type : GLenum
    {
        None = GL_NONE,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
    };
    
public:
    
    RenderBuffer(int width, int height, Type type);
    
public:
    
    Type BufferType;
};