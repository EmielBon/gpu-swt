//
//  Ray.h
//  OGLTest
//
//  Created by Emiel Bon on 13-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

struct Ray
{
public:
    
    float Length() const;
    
public:
    
    List<Point> Points;
};
