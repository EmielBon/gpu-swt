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

class FrameBuffer : public IOGLBindableResource<FrameBuffer>
{
public:
    
    FrameBuffer();
    
    FrameBuffer(Ptr<Texture> colorAttachment, Ptr<RenderBuffer> renderBufferAttachment);
    
    void Attach(Ptr<Texture> colorAttachment);
    
    void Attach(Ptr<RenderBuffer> renderBufferAttachment);
    
    bool IsFrameBufferComplete() const;

    // Warning! Clamps color values to [0,1]
    void CopyColorAttachment(const Texture &destination) const;
    
    cv::Vec4f ReadPixel(int x, int y) const;
    
private:
    
    void AssertFrameBufferComplete() const;
    
public:
    
    Ptr<Texture>      ColorAttachment0;
    Ptr<RenderBuffer> RenderBufferAttachment;
};

inline bool FrameBuffer::IsFrameBufferComplete() const
{
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

inline void FrameBuffer::AssertFrameBufferComplete() const
{
    if (!IsFrameBufferComplete())
        throw std::runtime_error("Framebuffer initialization failed");
}