//
//  Texture.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "IOGLBindableResource.h"

class Texture : public IOGLBindableResource<Texture>
{
protected:
    
    Texture(int width, int height, GLenum filteringType = GL_LINEAR);
    
public:
    
    Texture(const cv::Mat &image);
    
    Texture(int width, int height, GLenum format, GLenum type, GLenum filteringType);
    
    int GetWidth() const;
    
    int GetHeight() const;

    int GetColorChannels() const;
    
    void GetTextureImage(GLenum format, GLenum type, GLvoid *buffer);
    
    Ptr<Texture> GetEmptyClone() const;
    
    //Ptr<Texture> Clone() const;
    
private:
    
    template<typename T>
    void SetData(const cv::Mat &image, GLenum format, GLenum type);
    
private:
    
    int width, height;
    int colorChannels;

public:
    
    GLenum Format;
    GLenum Type;
    GLenum FilteringType;
};

inline int Texture::GetWidth() const
{
    return width;
}

inline int Texture::GetHeight() const
{
    return height;
}

inline int Texture::GetColorChannels() const
{
    return colorChannels;
}

inline void Texture::GetTextureImage(GLenum format, GLenum type, GLvoid *buffer)
{
    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, buffer);
    Unbind();
}

inline Ptr<Texture> Texture::GetEmptyClone() const
{
    return New<Texture>(width, height, Format, Type, FilteringType);
}

/*inline Ptr<Texture> Texture::Clone() const
{
    auto texture = GetEmptyClone();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
}*/

template<typename T>
inline void Texture::SetData(const cv::Mat &image, GLenum format, GLenum type)
{
    Format = format;
    Type   = type;
    
    List<T> pixelData;
    pixelData.resize(width * height);
    std::copy(image.begin<T>(), image.end<T>(), pixelData.begin());
    Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixelData.data());
    Unbind();
}
