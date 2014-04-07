//
//  FrameBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "FrameBuffer.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(int width, int height, GLenum format, GLenum type, RenderBuffer::Type attachment)
{
    glGenFramebuffers(1, &bufferId);
    
    Bind();
    
    if (attachment != RenderBuffer::Type::None)
    {
        Attachment = New<::RenderBuffer>(attachment, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)attachment, GL_RENDERBUFFER, Attachment->GetHandle());
    }
    
    // OpenGL ES only allows COLOR_ATTACHMENT0!
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    
    CreateNewColorAttachment0(width, height, format, type);
    AssertFrameBufferComplete();
    
    Unbind();
}

void FrameBuffer::SetColorAttachment0(Ptr<::Texture> texture)
{
    Texture = texture;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Texture->GetHandle(), 0);
    AssertFrameBufferComplete();
}

void FrameBuffer::CreateNewColorAttachment0()
{
    CreateNewColorAttachment0(Texture->GetWidth(), Texture->GetHeight(), Texture->Format, Texture->Type);
}

void FrameBuffer::CreateNewColorAttachment0(int width, int height, GLenum format, GLenum type)
{
    auto texture = New<::Texture>(width, height, format, type, GL_NEAREST);
    SetColorAttachment0(texture);
}