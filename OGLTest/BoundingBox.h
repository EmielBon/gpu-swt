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
    
    cv::Vec2i Center() const;
    
    int X() const;
    
    int Y() const;
    
    int Width() const;
    
    int Height() const;
    
public:
    
    cv::Rect Bounds;
};

inline BoundingBox::BoundingBox(const cv::Rect &bounds) : Bounds(bounds)
{
    
}

inline ContainmentType BoundingBox::Contains(const BoundingBox &other) const
{
    if (Bounds.contains(other.Bounds.tl()) && Bounds.contains(other.Bounds.br()))
        return ContainmentType::Contains;
    if (other.X() + other.Width() < X() ||
        other.X() + other.Width() > X() + Width() ||
        other.Y() + other.Height() < Y() ||
        other.Y() + other.Height() > Y() + Height())
        return ContainmentType::Disjoint;
    return ContainmentType::Intersects;
}

inline cv::Vec2i BoundingBox::Center() const
{
    return {Bounds.x + Bounds.width / 2, Bounds.y + Bounds.height / 2};
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