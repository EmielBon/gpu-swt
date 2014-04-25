//
//  IOGLResource.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "GLError.h"

class IOGLResource
{
public:
    
    virtual ~IOGLResource() = default;
    
    void Setup(Function<void(GLsizei, GLuint*)> generateFunction, Function<void(GLsizei, GLuint*)> disposeFunction);
    
    void Generate();
    
    GLuint GetHandle() const;
    
    void Dispose();
    
protected:
    
    GLuint handle;
    Function<void(GLsizei, GLuint*)> GenerateFunction;
    Function<void(GLsizei, GLuint*)> DisposeFunction;
};

inline void IOGLResource::Setup(Function<void(GLsizei, GLuint*)> generateFunction, Function<void(GLsizei, GLuint*)> disposeFunction)
{
    GenerateFunction = generateFunction;
    DisposeFunction  = disposeFunction;
}

inline void IOGLResource::Generate()
{
    GenerateFunction(1, &handle);
    check_gl_error();
}

inline GLuint IOGLResource::GetHandle() const
{
    return handle;
}

inline void IOGLResource::Dispose()
{
    DisposeFunction(1, &handle);
    check_gl_error();
}
