//
//  BoundingBox.h
//  OGLTest
//
//  Created by Emiel Bon on 28-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

enum class ContainmentType
{
    Disjoint,
    Intersects,
    Contains,
};

class BoundingBox
{
public:
    
    BoundingBox() = default;
    
    BoundingBox(const cv::Rect &bounds);
    
    ContainmentType Contains(const BoundingBox &other) const;
    
    Point Center() const;
    
    Point BaseLineCenter() const;
    
    int X() const;
    
    int Y() const;
    
    int Width() const;
    
    int Height() const;
    
    int Area() const;
    
public:
    
    cv::Rect Bounds;
};

inline BoundingBox::BoundingBox(const cv::Rect &bounds) : Bounds(bounds)
{
    
}

inline Point BoundingBox::Center() const
{
    return {Bounds.x + Bounds.width / 2, Bounds.y + Bounds.height / 2};
}

inline Point BoundingBox::BaseLineCenter() const
{
    return {Bounds.x + Bounds.width / 2, Bounds.y + Bounds.height};
}

inline int BoundingBox::X() const
{
    return Bounds.x;
}

inline int BoundingBox::Y() const
{
    return Bounds.y;
}

inline int BoundingBox::Width() const
{
    return Bounds.width;
}

inline int BoundingBox::Height() const
{
    return Bounds.height;
}

inline int BoundingBox::Area() const
{
    return Bounds.area();
}