//
//  FrameBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "FrameBuffer.h"
#include "DepthBuffer.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(int width, int height, GLenum format, GLenum type, bool hasDepthBuffer)
{
    glGenFramebuffers(1, &bufferId);
    
    Bind();
    
    if (hasDepthBuffer)
    {
        depthBuffer = New<DepthBuffer>(width, height);
        depthBuffer->Bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferId);
    }
    
    texture = New<Texture>(width, height, format, type, GL_NEAREST);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->GetHandle(), 0);
    
    // OpenGL ES only allows COLOR_ATTACHMENT0!
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer initialization failed");
    
    depthBuffer->Unbind();
    Unbind();
}