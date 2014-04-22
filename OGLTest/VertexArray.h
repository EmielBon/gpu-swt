//
//  VertexArray.h
//  OGLTest
//
//  Created by Emiel Bon on 04-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"
#include "types.h"
#include <GL/glew.h>

class VertexArray : public IOGLResource
{
public:
    
    VertexArray();
    
    void Bind();
    
    void Unbind();
    
public:
    
    GLuint vertexArrayObjectId;
};

inline VertexArray::VertexArray()
{
    Setup(glGenVertexArrays, glDeleteVertexArrays);
    Generate();
}

inline void VertexArray::Bind()
{
    glBindVertexArray( GetHandle() );
}

inline void VertexArray::Unbind()
{
    glBindVertexArray(0);
}