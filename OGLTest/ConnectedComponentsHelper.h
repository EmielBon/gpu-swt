//
//  ConnectedComponentsHelper.h
//  OGLTest
//
//  Created by Emiel Bon on 06-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class ConnectedComponentsHelper
{
public:
    
    static cv::Mat FindComponents(const cv::Mat &input, List< Ptr<Component> > &components, float bgValue);
};
