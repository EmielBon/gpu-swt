//
//  Filter.h
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class Filter
{
public:
    
    virtual Ptr<Texture> Apply() = 0;
};
