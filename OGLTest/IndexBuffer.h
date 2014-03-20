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

class IndexBuffer : public OGLBuffer
{
public:
    
    IndexBuffer();
    
    void Bind();
    
    void SetData(const List<GLubyte> &indices);
    
    GLsizei Count() const;
    
private:
    
    GLsizei count;
};

inline IndexBuffer::IndexBuffer() : count(0)
{
}

inline void IndexBuffer::Bind()
{
    BindTo(GL_ELEMENT_ARRAY_BUFFER);
}

inline void IndexBuffer::SetData(const List<GLubyte> &indices)
{
    count = (GLsizei)indices.size();
    OGLBuffer::SetData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLubyte), indices.data());
}

inline GLsizei IndexBuffer::Count() const
{
    return count;
}
