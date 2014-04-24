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
private:
    
    using base = Filter;
    
public:
    
    GrayFilter(GraphicsDevice *device, const Texture &input);
    
    void LoadShaderPrograms();
};
