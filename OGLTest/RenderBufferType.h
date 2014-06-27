//
//  RenderBufferType.h
//  OGLTest
//
//  Created by Emiel Bon on 23-06-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//
#pragma once

#include "types.h"

enum class RenderBufferType : GLenum
{
    None = GL_NONE,
    Depth = GL_DEPTH_ATTACHMENT,
    Stencil = GL_STENCIL_ATTACHMENT,
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
};