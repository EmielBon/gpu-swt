//
//  Texture.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Texture.h"

Texture::Texture(const TextureParameters &params, const GLvoid *pixels /* = nullptr */)
{
    Setup(glGenTextures, glDeleteTextures, glBindTexture, GL_TEXTURE_2D);
    Generate();
    Initialize(params, pixels);
}

void Texture::Initialize(const TextureParameters &params, const GLvoid *pixels /* = nullptr */)
{
    Parameters = params;
    
    Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Parameters.FilteringType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Parameters.FilteringType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    SetData(pixels); // Unbind happens here
}

void Texture::GetTextureImage(GLenum format, GLenum type, GLvoid *buffer) const
{
    Bind();
        glGetTexImage(GL_TEXTURE_2D, 0, format, type, buffer);
    Unbind();
}

void Texture::SetData(const GLvoid* pixels)
{
    Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GetWidth(), GetHeight(), 0, Parameters.Format, Parameters.Type, pixels);
    Unbind();
}