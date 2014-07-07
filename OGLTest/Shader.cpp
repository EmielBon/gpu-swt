//
//  Shader.cpp
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Shader.h"
#include "ContentLoader.h"

using namespace std;

Shader::Shader(const String &name, const String &sourceText, GLenum shaderType) : shaderId(0), name(name), source(sourceText)
{
    //create the shader object
    shaderId = glCreateShader(shaderType);
    if(shaderId == 0)
        throw runtime_error("glCreateShader failed");
    
    String versionDirective = "#version 150\n";
    fullSource = versionDirective + ResolveIncludes(source);
    
    //set the source code
    const char* code = fullSource.c_str();
    
    glShaderSource(shaderId, 1, (const GLchar**)&code, NULL);
    
    //compile
    glCompileShader(shaderId);
    
    //throw exception if compile error occurred
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        String msg = "Compile failure in shader " + name + ":\n";
        
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteShader(shaderId);
        shaderId = 0;
        throw runtime_error("\n" + msg);
    }
}

String Shader::ResolveIncludes(const String &source)
{
    String fullSource = source;
    String includeDirective = "#pragma include ";
    size_t found = fullSource.find(includeDirective);
    while (found != String::npos)
    {
        size_t startPos = found + includeDirective.length();
        size_t   endPos = fullSource.find("\n", startPos);
        String fileName = fullSource.substr(startPos, endPos - startPos);
        String includedFileText = ContentLoader::FileReadAll(ContentLoader::ContentPath + fileName);
        fullSource.replace(found, endPos - found, includedFileText);
        found = fullSource.find(includeDirective);
    }
    return fullSource;
}