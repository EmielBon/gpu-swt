//
//  Program.cpp
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Program.h"

#include "ContentLoader.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"

#include <GL/glew.h>

Program* Program::currentlyUsedProgram = nullptr;

Program::Program(const List< Ptr<Shader>> &shaders) : programId(0)
{
    programId = CreateFromShaders(shaders);
    AssertLinkingSuccess(programId);
    IndexUniforms();
}

GLuint Program::CreateFromShaders(const List<Ptr<Shader> > &shaders)
{
    if(shaders.size() == 0)
        throw std::runtime_error("No shaders were provided to create the program");
    
    //create the program object
    GLuint programId = glCreateProgram();
    if(programId == 0)
        throw std::runtime_error("glCreateProgram failed");
    
    //attach all the shaders
    for(unsigned i = 0; i < shaders.size(); ++i)
        glAttachShader(programId, shaders[i]->GetHandle());
    
    //link the shaders together
    glLinkProgram(programId);
    
    //detach all the shaders
    for(unsigned i = 0; i < shaders.size(); ++i)
        glDetachShader(programId, shaders[i]->GetHandle());
    
    return programId;
}

void Program::AssertLinkingSuccess(GLuint programId)
{
    //throw exception if linking failed
    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        String msg("Program linking failure: ");
        
        GLint infoLogLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programId, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteProgram(programId);
        programId = 0;
        
        throw std::runtime_error(msg);
    }
}

void Program::IndexUniforms()
{
    int total = -1;
    int textureIndex = 0;
    
    glGetProgramiv( programId, GL_ACTIVE_UNIFORMS, &total );
    for(int i = 0; i < total; ++i)  {
        int nameLength = -1, size = -1;
        GLenum type = GL_ZERO;
        char name[100];
        glGetActiveUniform( programId, (GLuint)i, sizeof(name)-1,
                           &nameLength, &size, &type, name );
        name[nameLength] = 0;
        GLuint location = glGetUniformLocation( programId, name );
        
        Uniforms[name] = Uniform( name, location, type, (type == GL_SAMPLER_2D) ? textureIndex++ : 0);
    }
}

GLint Program::GetAttributeLocation(const String &name) const
{
    GLint attrib = glGetAttribLocation(programId, name.c_str());
    
    if(attrib == -1)
        throw std::runtime_error(String("Program attribute not found: ") + name);
    
    return attrib;
}

void Program::Use()
{
    auto &vertexBuffer      = *GraphicsDevice::VertexBuffer;
    auto &vertexDeclaration = vertexBuffer.GetVertexDeclaration();
    auto &vertexArray       = vertexDeclaration.GetVertexArray();
    GLuint stride           = vertexDeclaration.Stride();
    
    vertexBuffer.Bind();
    vertexArray.Bind();
    
    auto &vertexDeclarationElements = vertexDeclaration.GetElements();
    
    // Can be optimized/avoided by caching
    for(auto vertexElement : vertexDeclarationElements)
    {
        GLint location = GetAttributeLocation(vertexElement.GetName().c_str());
        GLuint size = vertexElement.GetSize();
        GLenum type = vertexElement.GetType();
        
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, type, GL_FALSE, stride, (const GLvoid*)vertexElement.GetElementOffset());
    }
    
    vertexBuffer.Unbind();
    
    currentlyUsedProgram = this;
    glUseProgram(programId);
}

Ptr<Program> Program::LoadScreenSpaceProgram(const String &fragmentShaderSource)
{
    return ContentLoader::Load<Program>("Trivial", fragmentShaderSource);
}