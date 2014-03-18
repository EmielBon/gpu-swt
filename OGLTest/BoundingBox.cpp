//
//  BoundingBox.cpp
//  OGLTest
//
//  Created by Emiel Bon on 28-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "BoundingBox.h"

ContainmentType BoundingBox::Contains(const BoundingBox &other) const
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