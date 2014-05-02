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
    
    FrameBuffer(int width, int height, GLenum format, GLenum type, RenderBuffer::Type attachment = RenderBuffer::Type::None);
    
    void SetColorAttachment0(Ptr<::Texture> texture);

    bool IsFrameBufferComplete() const;

    void CreateNewColorAttachment0();
    
    Ptr<Texture> CopyColorAttachment(Ptr<::Texture> destination = nullptr) const;
    
private:

    void CreateNewColorAttachment0(int width, int height, GLenum format, GLenum type);
    
    void AssertFrameBufferComplete() const;
    
public:
    
    Ptr<::Texture> Texture;
    Ptr<RenderBuffer> Attachment;
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