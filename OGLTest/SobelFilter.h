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
    
    SobelFilter(Ptr<Texture> input = nullptr);
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps();

private:
    
    Ptr<Texture> ScharrAveraging(const Texture &input);
    
    Ptr<Texture> Differentiation(const Texture &input);
    
private:
    
    Ptr<Program> scharr, diff;
};

inline SobelFilter::SobelFilter(Ptr<Texture> input) : base("Sobel", input)
{
    
}

inline void SobelFilter::LoadShaderPrograms()
{
    scharr = LoadScreenSpaceProgram("Sobel1");
    diff   = LoadScreenSpaceProgram("Sobel2");
}

inline Ptr<Texture> SobelFilter::PerformSteps()
{
    return Differentiation( *ScharrAveraging( *Input) );
}