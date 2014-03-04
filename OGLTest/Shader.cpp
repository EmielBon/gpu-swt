//
//  Shader.cpp
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Shader.h"

using namespace std;

Shader::Shader(const String &sourceText, GLenum shaderType) : shaderId(0)
{
    //create the shader object
    shaderId = glCreateShader(shaderType);
    if(shaderId == 0)
        throw runtime_error("glCreateShader failed");
    
    //set the source code
    const char* code = sourceText.c_str();
    glShaderSource(shaderId, 1, (const GLchar**)&code, NULL);
    
    //compile
    glCompileShader(shaderId);
    
    //throw exception if compile error occurred
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        String msg("Compile failure in shader:\n");
        
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteShader(shaderId);
        shaderId = 0;
        throw runtime_error(msg);
    }
}