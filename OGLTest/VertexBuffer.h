//
//  VertexBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 04-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"
#include "types.h"
#include "VertexDeclaration.h"
#include <GL/glew.h>

class VertexBuffer : public IOGLResource
{
    friend class GraphicsDevice;
    
public:
    
    VertexBuffer();
    
    template<class T>
    void SetData(const List<T> &data);
    
    GLuint GetHandle() const;

    void Dispose();
    
    const VertexDeclaration& GetVertexDeclaration() const;
    
private:
    
    GLuint bufferId;
    VertexDeclaration vertexDeclaration;
};

inline VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &bufferId);
}

template<class T>
inline void VertexBuffer::SetData(const List<T> &data)
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    vertexDeclaration = T::VertexDeclaration();
}

inline GLuint VertexBuffer::GetHandle() const
{
    return bufferId;
}

inline void VertexBuffer::Dispose()
{
    glDeleteBuffers(1, &bufferId);
    bufferId = 0;
}

inline const VertexDeclaration& VertexBuffer::GetVertexDeclaration() const
{
    return vertexDeclaration;
}