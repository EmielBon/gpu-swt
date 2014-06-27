//
//  DepthBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "RenderBufferType.h"
#include "types.h"

class RenderBuffer : public IOGLBindableResource<RenderBuffer>
{
public:
    
    RenderBuffer(int width, int height, RenderBufferType type);
    
public:
    
    RenderBufferType BufferType;
};