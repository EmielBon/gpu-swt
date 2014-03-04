//
//  Component.h
//  OGLTest
//
//  Created by Emiel Bon on 24-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "BoundingBox.h"

struct Component
{
public:

    Component();
    
    void AddPoint(const SWTPoint &point);
    
    const List<SWTPoint>& Points() const;
    
    const BoundingBox& BoundingBox() /*const*/;
    
    float Mean();
    
    float Variance();
    
    float MeanColor();
    
private:
    
    void Update();
    
private:
    
    bool dirty;
    List<SWTPoint> points;
    cv::Vec3f meanColor;
    ::BoundingBox boundingBox;
    float mean;
    float variance;
};

inline Component::Component() : dirty(true), variance(0)
{
    
}

inline void Component::AddPoint(const SWTPoint &point)
{
    dirty = true;
    points.push_back(point);
}

inline const List<SWTPoint>& Component::Points() const
{
    return points;
}

inline const BoundingBox& Component::BoundingBox()// const
{
    Update();
    return boundingBox;
}

inline float Component::Mean()
{
    Update();
    return mean;
}

inline float Component::Variance()
{
    Update();
    return variance;
}