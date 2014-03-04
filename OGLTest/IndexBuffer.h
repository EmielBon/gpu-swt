//
//  IndexBuffer.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "IOGLResource.h"

class IndexBuffer : public IOGLResource
{
public:
    
    IndexBuffer(const List<GLubyte> &indices);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
    GLsizei Count() const;
    
private:
    
    GLsizei count;
    GLuint bufferId;
};

inline GLuint IndexBuffer::GetHandle() const
{
    return bufferId;
}

inline void IndexBuffer::Dispose()
{
    glDeleteBuffers(1, &bufferId);
}

inline GLsizei IndexBuffer::Count() const
{
    return count;
}
