//
//  IOGLBindableResource.h
//  OGLTest
//
//  Created by Emiel Bon on 20-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"
#include "types.h"

template<class T>
class IOGLBindableResource : public IOGLResource
{
public:
    
    void Setup(Function<void(GLsizei, GLuint*)> generateFunction, Function<void(GLsizei, GLuint*)> disposeFunction, Function<void(GLenum, GLuint)> bindFunction, GLenum targetName);
    
    void Bind() const;
    
    void Unbind() const;
    
    bool IsBound() const { return GetHandle() == boundBuffer; }

    static void BindDefault();
    
protected:
    
    static Function<void(GLenum, GLuint)> BindFunction;
    static GLenum TargetName;
    
private:
    
    static GLuint boundBuffer;
};

template<class T> GLuint IOGLBindableResource<T>::boundBuffer = 0;
template<class T> Function<void(GLenum, GLuint)> IOGLBindableResource<T>::BindFunction;
template<class T> GLenum IOGLBindableResource<T>::TargetName = GL_NONE;

template<class T>
inline void IOGLBindableResource<T>::Setup(Function<void(GLsizei, GLuint*)> generateFunction, Function<void(GLsizei, GLuint*)> disposeFunction, Function<void(GLenum, GLuint)> bindFunction, GLenum targetName)
{
    BindFunction = bindFunction;
    TargetName   = targetName;
    IOGLResource::Setup(generateFunction, disposeFunction);
}

template<class T>
inline void IOGLBindableResource<T>::Bind() const
{
    if (!IsBound())
    {
        boundBuffer = GetHandle();
        BindFunction( TargetName, GetHandle() );
        check_gl_error();
    }
}

template<class T>
inline void IOGLBindableResource<T>::Unbind() const
{
    if (IsBound())
        BindDefault();
}

template<class T>
inline void IOGLBindableResource<T>::BindDefault()
{
    BindFunction(TargetName, 0);
    boundBuffer = 0;
    check_gl_error();
}