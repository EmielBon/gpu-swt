//
//  Texture.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Texture.h"

int Texture::ActiveTextureCount = 0;
int Texture::PeakTextureCount = 0;
int Texture::RecycledTextureCount = 0;
Map<Tuple<int, int, GLenum, GLenum>, Stack<GLuint>> Texture::freeTextures;

Texture::Texture(const TextureParameters &params, const GLvoid *pixels /* = nullptr */) : Parameters(params)
{
    auto generateFunction = [&](GLsizei count, GLuint* textures)
    {
        glRecycleTexture(params, textures);
    };
    
    auto deleteFunction = [&](GLsizei count, GLuint* textures)
    {
        glDisposeTexture(params, textures);
    };
    
    PeakTextureCount = MAX(PeakTextureCount, ActiveTextureCount);
    
    Setup(generateFunction, deleteFunction, glBindTexture, GL_TEXTURE_2D);
    Generate();
    SetData(pixels);
}

void Texture::glRecycleTexture(const TextureParameters &params, GLuint* textures)
{
    auto key = std::make_tuple(params.Width, params.Height, params.Format, params.Type);
    auto& freeHandles = freeTextures[key];
    
    if (freeHandles.empty())
    {
        glGenTextures(1, textures);
        ActiveTextureCount++;
    }
    else
    {
        textures[0] = freeHandles.top();
        freeHandles.pop();
        RecycledTextureCount++;
    }
}

void Texture::glDisposeTexture(const TextureParameters &params, GLuint *textures)
{
    auto key = std::make_tuple(params.Width, params.Height, params.Format, params.Type);
    auto& freeHandles = freeTextures[key];
    
    if (freeTextures.size() < MAX_RETAINED_TEXTURES)
        freeHandles.push( textures[0] );
    else
    {
        glDeleteTextures(1, textures);
        ActiveTextureCount--;
    }
}

Ptr<Texture> Texture::GetEmptyClone() const
{
    return New<Texture>(Parameters);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GetWidth(), GetHeight(), 0, Parameters.Format, Parameters.Type, pixels);
    Unbind();
}