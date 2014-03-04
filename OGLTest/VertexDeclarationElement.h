//
//  VertexDeclarationElement.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class VertexDeclarationElement
{
public:
    
    VertexDeclarationElement(const String &name, GLuint size, GLenum type, GLuint elementSize);
    
    const String& GetName() const;
    
    GLuint GetSize() const;
    
    GLenum GetType() const;
    
    GLenum GetElementSize() const;
    
    GLuint GetByteSize() const;
    
    GLuint GetElementOffset() const;
    
    void SetElementOffset(GLuint offset);
    
private:
    
    String name;
    GLuint size;
    GLenum type;
    GLuint elementSize;
    GLuint elementOffset;
};

inline VertexDeclarationElement::VertexDeclarationElement(const String &name, GLuint size, GLenum type, GLuint elementSize)
    : name(name), size(size), type(type), elementSize(elementSize), elementOffset(0)
{
    
}

inline const String& VertexDeclarationElement::GetName() const
{
    return name;
}

inline GLuint VertexDeclarationElement::GetSize() const
{
    return size;
}

inline GLenum VertexDeclarationElement::GetType() const
{
    return type;
}

inline GLuint VertexDeclarationElement::GetElementSize() const
{
    return elementSize;
}

inline GLuint VertexDeclarationElement::GetByteSize() const
{
    return size * elementSize;
}

inline GLuint VertexDeclarationElement::GetElementOffset() const
{
    return elementOffset;
}

inline void VertexDeclarationElement::SetElementOffset(GLuint offset)
{
    elementOffset = offset;
}