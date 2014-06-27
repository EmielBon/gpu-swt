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
    
    TextureParameters(GLenum internalFormat, int width, int height, GLenum format, GLenum type);
    
public:
    
    GLenum InternalFormat;
    int Width, Height;
    GLenum Format, Type;
};

inline TextureParameters::TextureParameters() : TextureParameters(GL_NONE, 0, 0, GL_NONE, GL_NONE)
{
    
}

inline TextureParameters::TextureParameters(GLenum internalFormat, int width, int height, GLenum format, GLenum type)
    : InternalFormat(internalFormat), Width(width), Height(height), Format(format), Type(type)
{
    
}