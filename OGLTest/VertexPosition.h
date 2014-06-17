//
//  VertexPosition.h
//  OGLTest
//
//  Created by Emiel Bon on 17-06-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

struct VertexPosition
{
public:
    
    VertexPosition() = default;
    
    VertexPosition(const Vector3 &position);
    
    static const VertexDeclaration& VertexDeclaration();
    
public:
    
    Vector3 Position;
};

inline VertexPosition::VertexPosition(const Vector3 &position)
    : Position(position)
{
    
}
