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
#include "Pair.h"
#include <assert.h>

class Chain
{
public:
    
    Chain(const Set<Component*> &components);
    
    Chain(const Pair &pair);
    
    cv::Vec2f Direction() const;
    
    float PolarAngle() const;
    
    static Ptr<Chain> Merge(const Chain &chain1, const Chain &chain2);
    
public:
    
    Set<Component*> Components;
};

inline Chain::Chain(const Set<Component*> &components) : Components(components)
{
    
}

inline Chain::Chain(const Pair &pair)
{
    Components.insert(pair.Component1);
    Components.insert(pair.Component2);
}