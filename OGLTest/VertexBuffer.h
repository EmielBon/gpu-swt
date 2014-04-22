//
//  VertexBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 04-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "OGLBuffer.h"
#include "types.h"
#include "VertexDeclaration.h"
#include <GL/glew.h>

class VertexBuffer : public OGLBuffer<VertexBuffer>
{
    friend class GraphicsDevice;
    
public:
    
    VertexBuffer();
    
    template<class T>
    void SetData(const List<T> &data);
    
    const VertexDeclaration& GetVertexDeclaration() const;
    
private:
    
    VertexDeclaration vertexDeclaration;
};

inline VertexBuffer::VertexBuffer()
{
    Setup(GL_ARRAY_BUFFER);
    Generate();
}

template<class T>
inline void VertexBuffer::SetData(const List<T> &data)
{
    OGLBuffer::SetData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data());
    vertexDeclaration = T::VertexDeclaration();
}

inline const VertexDeclaration& VertexBuffer::GetVertexDeclaration() const
{
    return vertexDeclaration;
}