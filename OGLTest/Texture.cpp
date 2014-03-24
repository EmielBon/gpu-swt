//
//  Texture.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Texture.h"

Texture::Texture(int width, int height, GLenum filteringType) : width(width), height(height)
{
    glGenTextures(1, &textureId);
    
    Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    Unbind();
}

Texture::Texture(const cv::Mat &image) : Texture(image.size().width, image.size().height, GL_LINEAR)
{
    colorChannels = image.channels();
    
    switch(image.type())
    {
        case CV_8UC1  : SetData<uchar    >(image, GL_RED, GL_UNSIGNED_BYTE); break;
        case CV_8UC2  : SetData<cv::Vec2b>(image, GL_RG,  GL_UNSIGNED_BYTE); break;
        case CV_8UC3  : SetData<cv::Vec3b>(image, GL_BGR, GL_UNSIGNED_BYTE); break;
        case CV_32FC1 : SetData<float    >(image, GL_RED, GL_FLOAT); break;
        case CV_32FC2 : SetData<cv::Vec2f>(image, GL_RG,  GL_FLOAT); break;
        case CV_32FC3 : SetData<cv::Vec3f>(image, GL_BGR, GL_FLOAT); break;
        default: throw std::runtime_error("Incompatible source format for texture"); break;
    }
}

Texture::Texture(int width, int height, GLenum format, GLenum type, GLenum filteringType) : Texture(width, height, filteringType)
{
    Format = format;
    Type = type;
    
    switch(format)
    {
        case GL_RED : colorChannels = 1; break;
        case GL_RG  : colorChannels = 2; break;
        case GL_BGR : colorChannels = 3; break;
        case GL_RGB : colorChannels = 3; break;
        default : throw std::runtime_error("Incompatible format"); break;
    }
    
    Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, 0); // fill with 0's (last argument)
    Unbind();
}