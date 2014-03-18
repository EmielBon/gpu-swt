//
//  Chain.h
//  OGLTest
//
//  Created by Emiel Bon on 28-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "Component.h"
#include <assert.h>

class Chain
{
private:
    
    Chain();
    
public:
    
    Chain(const std::initializer_list< Ptr<Component> > &components);
    
    cv::Vec2f Direction(bool normalize = true) const;
    
    float PolarAngle() const;
    
    static Ptr<Chain> Merge(const Chain &chain1, const Chain &chain2);
    
    ::BoundingBox BoundingBox() const;
    
public:
    
    OrderedSet< Ptr<Component> > Components;
};

inline Chain::Chain()
{
    Components = OrderedSet< Ptr<Component> >(Component::HasSmallerPosition);
}