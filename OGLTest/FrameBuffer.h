//
//  FrameBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "types.h"

class FrameBuffer : public IOGLBindableResource
{
public:
    
    FrameBuffer(int width, int height, GLenum format, GLenum type, bool hasDepthBuffer);
    
    GLuint GetHandle() const;
    
    void Bind();
    
    void Unbind();
    
    void Dispose();
    
public:
    
    GLuint bufferId;
    Ptr<Texture> texture;
    Ptr<DepthBuffer> depthBuffer;
};

inline GLuint FrameBuffer::GetHandle() const
{
    return bufferId;
}

inline void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, bufferId);
}

inline void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void FrameBuffer::Dispose()
{
    glDeleteFramebuffers(1, &bufferId);
    bufferId = 0;
}