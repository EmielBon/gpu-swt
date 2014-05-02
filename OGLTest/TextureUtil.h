//
//  TextureUtil.h
//  OGLTest
//
//  Created by Emiel Bon on 01-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Texture.h"
#include "types.h"

template<class T>
Ptr<Texture> textureFromImage(const cv::Mat &image)
{
    GLenum format = GL_NONE;
    GLenum type   = GL_NONE;
    
    int width  = image.size().width;
    int height = image.size().height;
    
    switch(image.type())
    {
        case CV_8UC1  : format = GL_RED; type = GL_UNSIGNED_BYTE; break;
        case CV_8UC2  : format = GL_RG;  type = GL_UNSIGNED_BYTE; break;
        case CV_8UC3  : format = GL_BGR; type = GL_UNSIGNED_BYTE; break;
        case CV_32FC1 : format = GL_RED; type = GL_FLOAT;         break;
        case CV_32FC2 : format = GL_RG;  type = GL_FLOAT;         break;
        case CV_32FC3 : format = GL_BGR; type = GL_FLOAT;         break;
        default: throw std::runtime_error("Incompatible source format for texture"); break;
    }
    
    List<T> pixelData;
    pixelData.resize(width * height);
    std::copy(image.begin<T>(), image.end<T>(), pixelData.begin());
    
    // todo: make GL_NEAREST? Will that break gaussian blur & stuff?
    // Also, is the fact that integer pixels are fetched, and the fact that the real value lies in the pixel center the reason why averaging fails?
    return New<Texture>(width, height, format, type, GL_NEAREST, pixelData.data());
}