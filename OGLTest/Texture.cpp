//
//  Texture.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Texture.h"

Texture::Texture(const cv::Mat &image)
    : width( image.cols ), height( image.rows ), colorChannels(3)
{
    glGenTextures(1, &textureId);
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    switch(image.type())
    {
        case CV_8UC1:  SetData<uchar    >(image, GL_RED, GL_UNSIGNED_BYTE); break;
        case CV_8UC2:  SetData<cv::Vec2b>(image, GL_RG,  GL_UNSIGNED_BYTE); break;
        case CV_8UC3:  SetData<cv::Vec3b>(image, GL_BGR, GL_UNSIGNED_BYTE); break;
        case CV_32FC1: SetData<float    >(image, GL_RED, GL_FLOAT); break;
        case CV_32FC2: SetData<cv::Vec2f>(image, GL_RG,  GL_FLOAT); break;
        case CV_32FC3: SetData<cv::Vec3f>(image, GL_BGR, GL_FLOAT); break;
        default: throw std::runtime_error("Incompatible source format for texture");
    }
    
    colorChannels = image.channels();
    
    glBindTexture(GL_TEXTURE_2D, 0);
}