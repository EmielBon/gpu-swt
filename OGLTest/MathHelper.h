//
//  MathHelper.h
//  OGLTest
//
//  Created by Emiel Bon on 18-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include <algorithm>

class MathHelper
{
public:
    
    template<typename T>
    static T Mean(const List<T> &values)
    {
        T sum = 0;
        for(auto value : values)
            sum += value;
        return sum / values.size();
    }
    
    template<typename T>
    static T Median(const List<T> &values)
    {
        List<T> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        
        size_t size = sorted.size();
        
        return (size % 2 == 0) ? (sorted[size / 2] + sorted[size / 2 + 1]) / 2
                               :  sorted[size / 2];
    }
    
    template<typename T>
    static T GetRatio(T x, T y)
    {
        if (x < y)
            std::swap(x, y);
        return x / y;
    }
    
    static float Length(const cv::Vec2i &v)
    {
        return sqrt(v[0] * v[0] + v[1] * v[1]);
    }
};
