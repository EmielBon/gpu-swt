//
//  StrokeWidthTransform.h
//  OGLTest
//
//  Created by Emiel Bon on 13-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class SWTHelper
{
public:
    
    static List<BoundingBox> StrokeWidthTransform(const cv::Mat &input);
};