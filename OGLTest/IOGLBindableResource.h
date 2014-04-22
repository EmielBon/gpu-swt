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
    
    bool IsBound() const;

protected:
    
    Function<void(GLenum, GLuint)> BindFunction;
    GLenum TargetName;
    
private:
    
    static GLuint boundBuffer;
};

template<class T> GLuint IOGLBindableResource<T>::boundBuffer = 0;

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
    //if (!IsBound())
    //{
    boundBuffer = GetHandle();
    BindFunction( TargetName, GetHandle() );
    //}
}

template<class T>
inline void IOGLBindableResource<T>::Unbind() const
{
    //if (IsBound())
    //{
    boundBuffer = 0;
    BindFunction( TargetName, 0);
    //}
}

template<class T>
inline bool IOGLBindableResource<T>::IsBound() const
{
    return GetHandle() == boundBuffer;
}