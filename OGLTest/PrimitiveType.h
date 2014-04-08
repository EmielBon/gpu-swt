//
//  PrimitiveType.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

enum class PrimitiveType : GLenum
{
    Points = GL_POINTS,
    Lines = GL_LINES,
    Triangles = GL_TRIANGLES,
};
