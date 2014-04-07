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

class RenderBuffer : public IOGLBindableResource
{
public:
    
    enum class Type : GLenum
    {
        None = GL_NONE,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT,
        DepthStencil = GL_DEPTH24_STENCIL8,
    };
    
public:
    
    RenderBuffer(Type type, int width, int height);
    
    GLuint GetHandle() const;
    
    void Bind();
    
    void Unbind();
    
    void Dispose();
    
public:
    
    GLuint bufferId;
};

inline GLuint RenderBuffer::GetHandle() const
{
    return bufferId;
}

inline void RenderBuffer::Bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
}

inline void RenderBuffer::Unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline void RenderBuffer::Dispose()
{
    glDeleteRenderbuffers(1, &bufferId);
    bufferId = 0;
}