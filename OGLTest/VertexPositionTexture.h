//
//  VertexPositionTexture.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

struct VertexPositionTexture
{
public:
    
    VertexPositionTexture() = default;
    
    VertexPositionTexture(const Vector3 &position, const Vector2 &texCoord);
    
    static const VertexDeclaration& VertexDeclaration();
    
public:
    
    Vector3 Position;
    Vector2 TexCoord;
};

inline VertexPositionTexture::VertexPositionTexture(const Vector3 &position, const Vector2 &texCoord)
    : Position(position), TexCoord(texCoord)
{
    
}