//
//  Texture.h
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "IOGLResource.h"

class Texture : public IOGLResource
{
public:
    
    Texture(const cv::Mat &image);
    
    GLuint GetHandle() const;
    
    void Dispose();
    
    int GetWidth() const;
    
    int GetHeight() const;

    int GetColorChannels() const;
    
private:
    
    template<typename T>
    void SetData(const cv::Mat &image, GLenum format, GLenum type);
    
private:
    
    GLuint textureId;
    int width, height;
    int colorChannels;
};

inline GLuint Texture::GetHandle() const
{
    return textureId;
}

inline void Texture::Dispose()
{
    glDeleteTextures(1, &textureId);
}

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

template<typename T>
inline void Texture::SetData(const cv::Mat &image, GLenum format, GLenum type)
{
    List<T> pixelData;
    pixelData.resize(image.cols * image.rows);
    std::copy(image.begin<T>(), image.end<T>(), pixelData.begin());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixelData.data());
}
