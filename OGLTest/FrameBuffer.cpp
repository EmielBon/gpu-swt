//
//  FrameBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "FrameBuffer.h"
#include "Texture.h"

FrameBuffer::FrameBuffer()
{
    Setup(glGenFramebuffers, glDeleteFramebuffers, glBindFramebuffer, GL_FRAMEBUFFER);
    Generate();
    
    Bind();
    
    check_gl_error();
    
    // OpenGL ES only allows COLOR_ATTACHMENT0!
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
}

FrameBuffer::FrameBuffer(Ptr<Texture> colorAttachment, Ptr<RenderBuffer> renderBufferAttachment = nullptr) : FrameBuffer()
{
    Attach(colorAttachment);
    if (renderBufferAttachment)
        Attach(renderBufferAttachment);
}

void FrameBuffer::Attach(Ptr<Texture> colorAttachment)
{
    if (colorAttachment->Parameters.FilteringType != GL_NEAREST)
        printf("Warning: Color attachment has filteringtype != GL_NEAREST");
    
    ColorAttachment0 = colorAttachment;
    Bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ColorAttachment0->GetHandle(), 0);
    AssertFrameBufferComplete();
}

void FrameBuffer::Attach(Ptr<RenderBuffer> renderBufferAttachment)
{
    RenderBufferAttachment = renderBufferAttachment;
    Bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)(RenderBufferAttachment->BufferType), GL_RENDERBUFFER, RenderBufferAttachment->GetHandle());
}

void FrameBuffer::CopyColorAttachment(const Texture &dest) const
{
    auto prev = Texture::GetCurrentlyBound();
    
    dest.Bind();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dest.GetWidth(), dest.GetHeight());
    
    if (prev)
        prev->Bind();
}

cv::Vec4f FrameBuffer::ReadPixel(int x, int y) const
{
    cv::Vec4f pixel;
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel);
    return pixel;
}