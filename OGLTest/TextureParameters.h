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
    
    TextureParameters(int width, int height, GLenum format, GLenum type, GLenum filteringType);
    
public:
    
    int Width, Height;
    GLenum Format, Type;
    GLenum FilteringType;
};

inline TextureParameters::TextureParameters() : TextureParameters(0, 0, GL_NONE, GL_NONE, GL_NEAREST)
{
    
}

inline TextureParameters::TextureParameters(int width, int height, GLenum format, GLenum type, GLenum filteringType)
    : Width(width), Height(height), Format(format), Type(type), FilteringType(filteringType)
{
    
}
