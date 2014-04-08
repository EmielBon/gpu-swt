//
//  FrameBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "RenderBuffer.h"
#include "types.h"

class FrameBuffer : public IOGLBindableResource
{
public:
    
    FrameBuffer(int width, int height, GLenum format, GLenum type, RenderBuffer::Type attachment = RenderBuffer::Type::None);
    
    GLuint GetHandle() const;

    void Bind();

    void Unbind();

    void Dispose();

    void SetColorAttachment0(Ptr<::Texture> texture);

    bool IsFrameBufferComplete() const;

    void CreateNewColorAttachment0();
    
private:

    void CreateNewColorAttachment0(int width, int height, GLenum format, GLenum type);
    
    void AssertFrameBufferComplete() const;
    
public:
    
    GLuint bufferId;
    Ptr<::Texture> Texture;
    Ptr<RenderBuffer> Attachment;
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

inline bool FrameBuffer::IsFrameBufferComplete() const
{
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

inline void FrameBuffer::AssertFrameBufferComplete() const
{
    if (!IsFrameBufferComplete())
        throw std::runtime_error("Framebuffer initialization failed");
}