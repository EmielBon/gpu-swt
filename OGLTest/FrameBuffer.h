//
//  FrameBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "RenderBufferType.h"
#include "types.h"

class FrameBuffer : public IOGLBindableResource<FrameBuffer>
{
public:
    
    FrameBuffer();
    
    FrameBuffer(Ptr<Texture> colorAttachment, Ptr<RenderBuffer> renderBufferAttachment = nullptr);
    
    void SetColorAttachment(Ptr<Texture> colorAttachment);
    
    void SetDepthStencil(Ptr<Texture> depthStencil);
    
    void SetDepthStencil(Ptr<RenderBuffer> depthStencil);
    
    bool IsFrameBufferComplete() const;

    // Warning! Clamps color values to [0,1]
    void CopyColorAttachment(const Texture &destination) const;
    
    template<class T>
    static List<T> ReadPixels(int x, int y, int width, int height, GLenum format, GLenum type);
    
    static List<cv::Vec4f> ReadPixels(int x, int y, int width, int height);
    
    static cv::Vec4f ReadPixel(int x, int y);
    
    static List<GLubyte> ReadStencil(int x, int y, int width, int height);
    
    static List<GLfloat> ReadDepth(int x, int y, int width, int height);
    
    void Print();
    
    void Print(RenderBufferType renderBuffer, int rowCount = 0);
    
private:
    
    void AssertFrameBufferComplete() const;
    
public:
    
    Ptr<Texture> ColorAttachment0;
    Ptr<Texture> DepthStencil;
    Ptr<RenderBuffer> RenderBufferAttachment;
};

inline bool FrameBuffer::IsFrameBufferComplete() const
{
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

template<class T>
inline List<T> FrameBuffer::ReadPixels(int x, int y, int width, int height, GLenum format, GLenum type)
{
    List<T> pixels(width * height);
    glReadPixels(x, y, width, height, format, type, pixels.data());
    return pixels;
}

inline List<cv::Vec4f> FrameBuffer::ReadPixels(int x, int y, int width, int height)
{
    return ReadPixels<cv::Vec4f>(x, y, width, height, GL_RGBA, GL_FLOAT);
}

inline cv::Vec4f FrameBuffer::ReadPixel(int x, int y)
{
    return ReadPixels(x, y, 1, 1)[0];
}

inline List<GLubyte> FrameBuffer::ReadStencil(int x, int y, int width, int height)
{
    return ReadPixels<GLubyte>(x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE);
}

inline List<GLfloat> FrameBuffer::ReadDepth(int x, int y, int width, int height)
{
    return ReadPixels<GLfloat>(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
}