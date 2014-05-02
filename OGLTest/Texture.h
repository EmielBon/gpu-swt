//
//  Texture.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLBindableResource.h"
#include "TextureParameters.h"
#include "types.h"

class Texture : public IOGLBindableResource<Texture>
{
private:
    
    using base = IOGLBindableResource<Texture>;
    
protected:
    
    // Empty constructor, meant to be overridden by subclasses
    Texture() = default;
    
public:
    
    Texture(int width, int height, GLenum format, GLenum type, GLenum filteringType, const GLvoid* pixels = NULL);
    
    Texture(const TextureParameters &params, const GLvoid* pixels = nullptr);
    
    virtual ~Texture() = default;
    
    // Initialize this texture with parameters and data. Parameters have to be set before calling this function
    void Initialize(const TextureParameters &params, const GLvoid *pixels = nullptr);
    
    int GetWidth() const;
    
    int GetHeight() const;

    void GetTextureImage(GLenum format, GLenum type, GLvoid *buffer) const;
    
    Ptr<Texture> GetEmptyClone() const;
    
    void SetData(const GLvoid* pixels);
    
public:
    
    TextureParameters Parameters;
    static const GLenum INTERNAL_FORMAT = GL_RGBA;
    static const GLenum PREFERRED_TYPE  = GL_UNSIGNED_INT_8_8_8_8_REV;
};

inline Texture::Texture(int width, int height, GLenum format, GLenum type, GLenum filteringType, const GLvoid *pixels /* = nullptr */)
    : Texture( TextureParameters(width, height, format, type, filteringType), pixels )
{
    
}

inline int Texture::GetWidth() const
{
    return Parameters.Width;
}

inline int Texture::GetHeight() const
{
    return Parameters.Height;
}

inline Ptr<Texture> Texture::GetEmptyClone() const
{
    return New<Texture>(Parameters);
}