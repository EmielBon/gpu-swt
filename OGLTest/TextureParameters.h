//
//  TextureParameters.h
//  OGLTest
//
//  Created by Emiel Bon on 01-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

struct TextureParameters
{
public:
 
    TextureParameters();
    
    TextureParameters(int width, int height, GLenum format, GLenum type);
    
public:
    
    int Width, Height;
    GLenum Format, Type;
};

inline TextureParameters::TextureParameters() : TextureParameters(0, 0, GL_NONE, GL_NONE)
{
    
}

inline TextureParameters::TextureParameters(int width, int height, GLenum format, GLenum type)
    : Width(width), Height(height), Format(format), Type(type)
{
    
}
