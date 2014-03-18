//
//  VertexDeclaration.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "VertexDeclarationElement.h"
#include "VertexArray.h"

class VertexDeclaration
{
public:
    
    VertexDeclaration();
    
    void AddElement(const VertexDeclarationElement& element);
    
    const List<VertexDeclarationElement>& GetElements() const;
    
    const VertexArray& GetVertexArray() const;
    
    GLuint Stride() const;
    
private:
    
    List<VertexDeclarationElement> elements;
    Ptr<VertexArray> vertexArray;
};

inline VertexDeclaration::VertexDeclaration()
{
    vertexArray = New<VertexArray>();
}

inline void VertexDeclaration::AddElement(const VertexDeclarationElement &element)
{
    elements.push_back(element);
    
    size_t size = elements.size();
    
    auto &current = elements[size - 1];
    auto &previous = elements[size - 2];
    
    if (size == 1)
        current.SetElementOffset(0);
    else
        current.SetElementOffset(previous.GetElementOffset() + previous.GetByteSize());
}

inline const List<VertexDeclarationElement>& VertexDeclaration::GetElements() const
{
    return elements;
}

inline const VertexArray& VertexDeclaration::GetVertexArray() const
{
    return *vertexArray;
}

inline GLuint VertexDeclaration::Stride() const
{
    GLuint sum = 0;
    for(auto element : elements)
        sum += element.GetByteSize();
    return sum;
}