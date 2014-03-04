//
//  Image.cpp
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Image.h"

Image::Image(const cv::Mat &img) : image(img)
{
    pixelData.resize(img.cols * img.rows * 3); // 3 bytes per pixel
    
    // optimize with std::copy or memcpy directly to vector's data() property
    GLubyte* pixel = &pixelData.front();
    for(auto it = image.begin<cv::Vec3b>(); it != image.end<cv::Vec3b>(); ++it)
    {
        *pixel = (*it)[2]; ++pixel;
        *pixel = (*it)[1]; ++pixel;
        *pixel = (*it)[0]; ++pixel;
    }
}