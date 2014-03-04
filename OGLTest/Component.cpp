//
//  Component.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Component.h"
#include <opencv2/imgproc/imgproc.hpp>

void Component::Update()
{
    if (!dirty) return;
    List<Point> positions;
    positions.reserve(points.size());
    float sum = 0;
    for(auto &p : points)
    {
        positions.push_back(p.Point);
        sum += p.SWTValue;
    }
    mean = sum / points.size();
    boundingBox = cv::boundingRect(positions);
    sum = 0;
    for(auto &p : points)
    {
        float d = p.SWTValue - mean;
        sum += d * d;
    }
    variance = sum / points.size();
    dirty = false;
}

