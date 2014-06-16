//
//  Shader.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"
#include "types.h"
#include <GLFW/glfw3.h>
#include <GL/glew.h>

class Shader
{
public:
    
    Shader(const String &name, const String &sourceText, GLenum shaderType);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
    const String& GetSource() const;
    
    const String& GetFullSource() const;
    
    String ResolveIncludes(const String &source);
    
private:
    
    GLuint shaderId;
    String name, source, fullSource;
};

inline GLuint Shader::GetHandle() const
{
    return shaderId;
}

inline void Shader::Dispose()
{
    glDeleteShader(shaderId);
    shaderId = 0;
}

inline const String& Shader::GetSource() const
{
    return source;
}

inline const String& Shader::GetFullSource() const
{
    return fullSource;
}