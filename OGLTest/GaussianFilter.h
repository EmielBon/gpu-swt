//
//  GaussianFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class GaussianFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    GaussianFilter();
    
    Ptr<Texture> PerformSteps(const Texture &input);
    
private:
    
    Ptr<Texture> HorizontalPass(const Texture &input);
    
    Ptr<Texture> VerticalPass(const Texture &input);
    
private:
    
    Ptr<Program> hor, ver;
};

inline GaussianFilter::GaussianFilter() : base("Gaussian Blur")
{
    hor = LoadScreenSpaceProgram("GaussianBlurH");
    ver = LoadScreenSpaceProgram("GaussianBlurV");
}

inline Ptr<Texture> GaussianFilter::PerformSteps(const Texture &input)
{
    return VerticalPass( *HorizontalPass(input) );
}