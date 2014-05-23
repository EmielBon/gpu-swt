//
//  TextRegionsFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 23-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class TextRegionsFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    TextRegionsFilter(Ptr<Texture> input = nullptr);
    
    void LoadShaderPrograms();
    
    void Initialize();
    
    void PerformSteps(Ptr<Texture> output);
    
public:
    
    ::GradientDirection GradientDirection;
    
private:
    
    Ptr<Filter> grayFilter, connectedComponentsFilter;
    Ptr<SWTFilter> swtFilter;
    Ptr<Texture> gray;
};

inline TextRegionsFilter::TextRegionsFilter(Ptr<Texture> input)
    : base("TextRegions", input), GradientDirection(GradientDirection::Unspecified)
{
    
}