//
//  FrameBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "FrameBuffer.h"
#include "Texture.h"
#include "GLError.h"
#include "RenderBuffer.h"

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

FrameBuffer::FrameBuffer(Ptr<Texture> colorAttachment, Ptr<RenderBuffer> depthStencil) : FrameBuffer()
{
    SetColorAttachment(colorAttachment);
    if (depthStencil)
        SetDepthStencil(depthStencil);
}

void FrameBuffer::AssertFrameBufferComplete() const
{
    if (!IsFrameBufferComplete())
    {
        check_gl_error();
        throw std::runtime_error("Framebuffer initialization failed");
    }
}

void FrameBuffer::SetColorAttachment(Ptr<Texture> colorAttachment)
{
    if (!colorAttachment)
        throw std::runtime_error("Attempt to attach nullptr color attachment");
        
    ColorAttachment0 = colorAttachment;
    Bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ColorAttachment0->GetHandle(), 0);
    AssertFrameBufferComplete();
}

void FrameBuffer::SetDepthStencil(Ptr<Texture> depthStencil)
{
    if (!depthStencil)
        throw std::runtime_error("Attempt to attach nullptr color attachment");
    
    DepthStencil = depthStencil;
    Bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencil->GetHandle(), 0);
    AssertFrameBufferComplete();
}

void FrameBuffer::SetDepthStencil(Ptr<RenderBuffer> renderBufferAttachment)
{
    if (!renderBufferAttachment)
        throw std::runtime_error("Attempt to attach nullptr renderbuffer attachment");
    
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

void FrameBuffer::Print()
{
    auto pixels = ReadPixels(0, 0, ColorAttachment0->GetWidth(), ColorAttachment0->GetHeight());
    for(auto pixel : pixels)
        printf("(%.2f,%.2f,%.2f,%.2f)", pixel[0], pixel[1], pixel[2], pixel[3]);
}

void FrameBuffer::Print(RenderBufferType renderBuffer, int rowCount)
{
    int width  = ColorAttachment0->GetWidth();
    int height = (rowCount == 0) ? ColorAttachment0->GetHeight() : rowCount;
    
    if (renderBuffer == RenderBufferType::Depth)
    {
        auto pixels = ReadDepth(0, 0, width, height);
        for(auto pixel : pixels)
            printf("%f ", pixel);
    }
    if (renderBuffer == RenderBufferType::Stencil)
    {
        auto pixels = ReadStencil(0, 0, width, height);
        for(auto pixel : pixels)
            printf("%u ", pixel);
    }
}