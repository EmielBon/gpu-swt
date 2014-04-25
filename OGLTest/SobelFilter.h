//
//  SobelFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class SobelFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    SobelFilter();
    
    Ptr<Texture> PerformSteps(const Texture &input);

private:
    
    Ptr<Texture> ScharrAveraging(const Texture &input);
    
    Ptr<Texture> Differentiation(const Texture &input);
    
private:
    
    Ptr<Program> scharr, diff;
};

inline SobelFilter::SobelFilter() : base("Sobel")
{
    scharr = LoadScreenSpaceProgram("Sobel1");
    diff   = LoadScreenSpaceProgram("Sobel2");
}

inline Ptr<Texture> SobelFilter::PerformSteps(const Texture &input)
{
    return Differentiation( *ScharrAveraging(input) );
}