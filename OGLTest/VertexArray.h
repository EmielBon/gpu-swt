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
    
    GLuint GetHandle() const;
    
    void Bind();
    
    void Unbind();
    
    void Dispose();
    
public:
    
    GLuint vertexArrayObjectId;
};

inline VertexArray::VertexArray()
{
    glGenVertexArrays(1, &vertexArrayObjectId);
}

inline GLuint VertexArray::GetHandle() const
{
    return vertexArrayObjectId;
}

inline void VertexArray::Bind()
{
    glBindVertexArray( GetHandle() );
}

inline void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

inline void VertexArray::Dispose()
{
    glDeleteVertexArrays(1, &vertexArrayObjectId);
}