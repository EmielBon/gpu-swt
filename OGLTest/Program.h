//
//  Program.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"
#include "types.h"
#include "Uniform.h"
#include <GL/glew.h>

class Program : public IOGLResource
{
public:
    
    Program(GraphicsDevice *device, const List< Ptr<Shader> > &shaders);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
    GLint GetAttributeLocation(const String &name) const;
    
    void Apply();
    
private:
    
    static GLuint CreateFromShaders(const List< Ptr<Shader> > &shaders);
    
    static void AssertLinkingSuccess(GLuint programId);
    
    void IndexUniforms();
    
private:
    
    GLuint programId;
    GraphicsDevice *device;
    
public:
    
    Map<String, Uniform> Uniforms;
};

inline GLuint Program::GetHandle() const
{
    return programId;
}

inline void Program::Dispose()
{
    glDeleteProgram(programId);
}