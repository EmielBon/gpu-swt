//
//  Pair.h
//  OGLTest
//
//  Created by Emiel Bon on 28-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "Component.h"

class Pair
{
public:
    
    Pair(Component *c1, Component *c2);
    
public:
    
    Component *Component1;
    Component *Component2;
};

inline Pair::Pair(Component *c1, Component *c2) : Component1(c1), Component2(c2)
{
    
}