//
//  Chain.cpp
//  OGLTest
//
//  Created by Emiel Bon on 28-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Chain.h"

cv::Vec2f Chain::Direction() const
{
    Component* left = *Components.begin();
    Component* right = *std::prev(Components.end());
    
    for(auto component : Components)
    {
        if (component->BoundingBox().Center()[0] < left->BoundingBox().Center()[0])
            left = component;
        else if (component->BoundingBox().Center()[0] > right->BoundingBox().Center()[0])
            right = component;
        else if (component->BoundingBox().Center()[0] == right->BoundingBox().Center()[0])
        {
            if (component->BoundingBox().Center()[1] < right->BoundingBox().Center()[1])
                left = component;
            else
                right = component;
        }
    }
    
    Point p1 = left->BoundingBox().Center();
    p1[1]   += left->BoundingBox().Height() / 2;
    
    Point p2 = right->BoundingBox().Center();
    p2[1]   += right->BoundingBox().Height() / 2;
    
    assert(left != right);
    
    cv::Vec2f direction = {p2[0] - p1[0], p2[1] - p1[1]};
    return cv::normalize(direction);
}

float Chain::PolarAngle() const
{
    Component* left = *Components.begin();
    Component* right = *std::prev(Components.end());
    
    for(auto component : Components)
    {
        if (component->BoundingBox().Center()[0] < left->BoundingBox().Center()[0])
            left = component;
        else if (component->BoundingBox().Center()[0] > right->BoundingBox().Center()[0])
            right = component;
        else if (component->BoundingBox().Center()[0] == right->BoundingBox().Center()[0])
        {
            if (component->BoundingBox().Center()[1] < right->BoundingBox().Center()[1])
                left = component;
            else
                right = component;
        }
    }
    
    Point p1 = left->BoundingBox().Center();
    p1[1]   += left->BoundingBox().Height() / 2;
    
    Point p2 = right->BoundingBox().Center();
    p2[1]   += right->BoundingBox().Height() / 2;
    
    assert(left != right);
    
    cv::Vec2f direction = {p2[0] - p1[0], p2[1] - p1[1]};
    
    return atan2f(direction[1], direction[0]);
}

Ptr<Chain> Chain::Merge(const Chain &chain1, const Chain &chain2)
{
    Set<Component*> merged;
    merged.insert(chain1.Components.begin(), chain1.Components.end());
    merged.insert(chain2.Components.begin(), chain2.Components.end());
    return New<Chain>(merged);
}
