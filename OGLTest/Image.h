//
//  Image.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class Image
{
public:
    
    Image(const cv::Mat &img);
    
    const GLubyte* GetPixelData() const;
    
    GLuint GetWidth() const;
    
    GLuint GetHeight() const;
    
    const cv::Vec3b& GetPixel(int x, int y) const;
    
    const GLubyte* GetPixelPtr(int x, int y) const;
    
private:
    
    cv::Mat image;
    List<GLubyte> pixelData;
};

inline const GLubyte* Image::GetPixelData() const
{
    return pixelData.data();
}

inline GLuint Image::GetWidth() const
{
    return image.cols;
}

inline GLuint Image::GetHeight() const
{
    return image.rows;
}

inline const cv::Vec3b& Image::GetPixel(int x, int y) const
{
    return image.at<cv::Vec3b>(y, x);
}

inline const GLubyte* Image::GetPixelPtr(int x, int y) const
{
    int i = (x + y * GetWidth()) * 3;
    return &(pixelData[i]);
}