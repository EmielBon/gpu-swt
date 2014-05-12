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
    
    void PerformSteps(Ptr<Texture> output);

private:
    
    void ScharrAveraging(const Texture &input, Ptr<Texture> output);
    
    void Differentiation(const Texture &input, Ptr<Texture> output);
    
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