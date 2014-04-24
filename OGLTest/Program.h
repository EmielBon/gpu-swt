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

class Program
{
public:
    
    Program(const List< Ptr<Shader> > &shaders);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
    GLint GetAttributeLocation(const String &name) const;
    
    void Use();
    
    static void UseDefault();
    
    static Ptr<Program> Load(const String &name);
    
    static Ptr<Program> LoadFromSources(const String &vertexShaderSource, const String &fragmentShaderSource);
    
private:
    
    static GLuint CreateFromShaders(const List< Ptr<Shader> > &shaders);
    
    static void AssertLinkingSuccess(GLuint programId);
    
    void IndexUniforms();
    
private:
    
    GLuint programId;
    
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

inline void Program::UseDefault()
{
    glUseProgram(0);
}

inline Ptr<Program> Program::Load(const String &name)
{
    return LoadFromSources(name, name);
}