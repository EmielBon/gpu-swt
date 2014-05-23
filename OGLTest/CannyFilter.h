//
//  CannyFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class CannyFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    CannyFilter(Ptr<Texture> input = nullptr);
    
    void LoadShaderPrograms();
    
    void Initialize();
    
    void PerformSteps(Ptr<Texture> output);
    
private:
    
    void DetectEdges(const Texture &gradients);
    
    void ScharrAveraging(const Texture &input, Ptr<Texture> output);
    
    void Differentiation(const Texture &input, Ptr<Texture> output);
    
    void PrepareStencilTest();
    
private:
    
    Ptr<Program> canny, scharr, diffCanny;
};

inline CannyFilter::CannyFilter(Ptr<Texture> input) : base("Canny", input)
{
    
}