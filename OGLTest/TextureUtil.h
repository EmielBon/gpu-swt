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
    
    cv::Mat floatImage;
    image.convertTo(floatImage, CV_32F, 1 / 255.0f);
    
    int width  = image.size().width;
    int height = image.size().height;
    
    switch(floatImage.type())
    {
        //case CV_8UC1  : format = GL_RED; type = GL_UNSIGNED_BYTE; break;
        //case CV_8UC2  : format = GL_RG;  type = GL_UNSIGNED_BYTE; break;
        //case CV_8UC3  : format = GL_BGR; type = GL_UNSIGNED_BYTE; break;
        case CV_32FC1 : format = GL_RED; type = GL_FLOAT;         break;
        case CV_32FC2 : format = GL_RG;  type = GL_FLOAT;         break;
        case CV_32FC3 : format = GL_BGR; type = GL_FLOAT;         break;
        default: throw std::runtime_error("Incompatible source format for texture"); break;
    }
    
    List<T> pixelData;
    pixelData.resize(width * height);
    std::copy(floatImage.begin<T>(), floatImage.end<T>(), pixelData.begin());
    
    // todo: make GL_NEAREST? Will that break gaussian blur & stuff?
    // Also, is the fact that integer pixels are fetched, and the fact that the real value lies in the pixel center the reason why averaging fails?
    return New<Texture>(GL_RGBA, width, height, format, type, pixelData.data());
}