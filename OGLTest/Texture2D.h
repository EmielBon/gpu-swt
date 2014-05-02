//
//  Texture2D.h
//  OGLTest
//
//  Created by Emiel Bon on 02-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Texture.h"
#include "types.h"

template<int Width, int Height, GLenum Format, GLenum Type, GLenum FilteringType>
class Texture2D : public Texture
{
private:
    
    using base = Texture;

public:
    
    Texture2D(const GLvoid *pixels = nullptr)
    {
        auto generateFunction = [&](GLsizei count, GLuint* textures)
        {
            if (freeTextures.empty())
                glGenTextures(count, textures);
            else
            {
                textures[0] = freeTextures.top();
                freeTextures.pop();
            }
        };
        
        auto deleteFunction = [](GLsizei count, GLuint* textures)
        {
            if (freeTextures.size() < MAX_RETAINED_TEXTURES)
                freeTextures.push( textures[0] );
            else
                glDeleteTextures(count, textures);
        };
        
        Setup(generateFunction, deleteFunction, glBindTexture, GL_TEXTURE_2D);
        Generate();
        Initialize(TextureParameters(Width, Height, Format, Type, FilteringType), pixels);
    }
    
    virtual ~Texture2D() = default;

private:

    static Stack<GLuint> freeTextures;
    static const int MAX_RETAINED_TEXTURES = 10;
};

template<int Width, int Height, GLenum Format, GLenum Type, GLenum FilteringType>
Stack<GLuint> Texture2D<Width, Height, Format, Type, FilteringType>::freeTextures;