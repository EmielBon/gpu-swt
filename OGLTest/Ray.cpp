//
//  Ray.cpp
//  OGLTest
//
//  Created by Emiel Bon on 13-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Ray.h"

float Ray::Length() const
{
    if (Points.size() < 2)
        return 0;
    auto &p = Points.front();
    auto &q = Points.back();
    float x = p[0] - q[0];
    float y = p[1] - q[1];
    return sqrt(x * x + y * y);
}