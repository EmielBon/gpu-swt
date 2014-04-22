//
//  GrayFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class GrayFilter : public Filter
{
public:
    
    GrayFilter(Ptr<Texture> input);
    
    Ptr<Texture> Apply(FrameBuffer* target);
    
    
};
