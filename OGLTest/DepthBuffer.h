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

class DepthBuffer : public IOGLBindableResource
{
public:
    
    DepthBuffer(int width, int height);
    
    GLuint GetHandle() const;
    
    void Bind();
    
    void Unbind();
    
    void Dispose();
    
public:
    
    GLuint bufferId;
};

inline GLuint DepthBuffer::GetHandle() const
{
    return bufferId;
}

inline void DepthBuffer::Bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
}

inline void DepthBuffer::Unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline void DepthBuffer::Dispose()
{
    glDeleteRenderbuffers(1, &bufferId);
    bufferId = 0;
}