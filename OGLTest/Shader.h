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

class Shader : public IOGLResource
{
public:
    
    Shader(const String &sourceText, GLenum shaderType);
    
    static Ptr<Shader> createFromFile(const String &filePath, GLenum shaderType);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
private:
    
    GLuint shaderId;
};

inline void Shader::Dispose()
{
    glDeleteShader(shaderId);
    shaderId = 0;
}

inline GLuint Shader::GetHandle() const
{
    return shaderId;
}