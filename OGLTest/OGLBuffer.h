//
//  OGLBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "types.h"

class OGLBuffer : public IOGLBindableResource
{
protected:
    
    OGLBuffer();
    
public:
    
    GLuint GetHandle() const;
    
    void BindTo(GLenum target);
    
    void SetData(GLenum target, size_t size, const void *data);
    
    void Unbind();
    
    bool IsBound();
    
    void Dispose();
    
protected:
    
    GLuint bufferId;
    GLenum boundTarget;
};

inline OGLBuffer::OGLBuffer() : boundTarget(GL_NONE)
{
    glGenBuffers(1, &bufferId);
}

inline GLuint OGLBuffer::GetHandle() const
{
    return bufferId;
}

inline void OGLBuffer::BindTo(GLenum target)
{
    glBindBuffer(target, bufferId);
    boundTarget = target;
}

inline void OGLBuffer::SetData(GLenum target, size_t size, const void *data)
{
    Bind();
    glBufferData(target, size, data, GL_STATIC_DRAW);
    Unbind();
}

inline void OGLBuffer::Unbind()
{
    if (IsBound())
        glBindBuffer(boundTarget, 0);
    boundTarget = GL_NONE;
}

inline bool OGLBuffer::IsBound()
{
    return boundTarget != GL_NONE;
}

inline void OGLBuffer::Dispose()
{
    glDeleteBuffers(1, &bufferId);
    bufferId = 0;
}

