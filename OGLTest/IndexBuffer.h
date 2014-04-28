//
//  IndexBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "OGLBuffer.h"
#include "PrimitiveType.h"

class IndexBuffer : public OGLBuffer<IndexBuffer>
{
private:
    
    using base = OGLBuffer<IndexBuffer>;
    
public:
    
    IndexBuffer(PrimitiveType primitiveType = PrimitiveType::Triangles);
    
    void SetData(const List<GLuint> &indices);
    
public:
    
    ::PrimitiveType PrimitiveType;
};

inline IndexBuffer::IndexBuffer(::PrimitiveType primitiveType /* = PrimitiveType::Triangles */) : PrimitiveType(primitiveType)
{
    Setup(GL_ELEMENT_ARRAY_BUFFER);
    Generate();
}

inline void IndexBuffer::SetData(const List<GLuint> &indices)
{
    base::SetData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizei>(indices.size()), sizeof(GLuint), indices.data());
}