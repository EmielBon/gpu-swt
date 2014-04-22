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

class IndexBuffer : public OGLBuffer<IndexBuffer>
{
public:
    
    IndexBuffer();
    
    void SetData(const List<GLuint> &indices);
    
    GLsizei Count() const;
    
private:
    
    GLsizei count;
};

inline IndexBuffer::IndexBuffer() : count(0)
{
    Setup(GL_ELEMENT_ARRAY_BUFFER);
    Generate();
}

inline void IndexBuffer::SetData(const List<GLuint> &indices)
{
    count = (GLsizei)indices.size();
    OGLBuffer::SetData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices.data());
}

inline GLsizei IndexBuffer::Count() const
{
    return count;
}
