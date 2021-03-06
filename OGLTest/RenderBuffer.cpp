//
//  DepthBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(int width, int height, RenderBufferType type)
{
    Setup(glGenRenderbuffers, glDeleteRenderbuffers, glBindRenderbuffer, GL_RENDERBUFFER);
    Generate();
    
    BufferType = type;
    GLenum storageFormat = GL_NONE;
    
    switch(type)
    {
        case RenderBufferType::Depth:        storageFormat = GL_DEPTH_COMPONENT24; break;
        case RenderBufferType::Stencil:      storageFormat = GL_STENCIL_INDEX8;    break;
        case RenderBufferType::DepthStencil: storageFormat = GL_DEPTH24_STENCIL8;  break;
        default: break;
    }
    
    Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, storageFormat, width, height);
    Unbind();
}