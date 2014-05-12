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
    
    GaussianFilter(Ptr<Texture> input = nullptr);
    
    void LoadShaderPrograms();
    
    void PerformSteps(Ptr<Texture> output);
    
private:
    
    void HorizontalPass(const Texture &input, Ptr<Texture> output);
    
    void VerticalPass(const Texture &input, Ptr<Texture> output);
    
private:
    
    Ptr<Program> hor, ver;
};

inline GaussianFilter::GaussianFilter(Ptr<Texture> input) : base("Gaussian Blur", input)
{
    
}

inline void GaussianFilter::LoadShaderPrograms()
{
    hor = LoadScreenSpaceProgram("GaussianBlurH");
    ver = LoadScreenSpaceProgram("GaussianBlurV");
}