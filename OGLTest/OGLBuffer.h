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

template<class T>
class OGLBuffer : public IOGLBindableResource<T>
{
public:
    
    void Setup(GLenum target);
    
    void SetData(GLenum target, size_t size, const void *data);
};

template<class T>
inline void OGLBuffer<T>::Setup(GLenum target)
{
    IOGLBindableResource<T>::Setup(glGenBuffers, glDeleteBuffers, glBindBuffer, target);
}

template<class T>
inline void OGLBuffer<T>::SetData(GLenum target, size_t size, const void *data)
{
    IOGLBindableResource<T>::Bind();
    glBufferData(target, size, data, GL_STATIC_DRAW);
    check_gl_error();
    IOGLBindableResource<T>::Unbind();
}