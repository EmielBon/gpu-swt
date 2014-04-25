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
    
    CannyFilter();
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps(const Texture &input);
    
private:
    
    Ptr<Texture> DetectEdges(const Texture &gradients);
    
    Ptr<Texture> Gradients(const Texture &input);
    
    Ptr<Texture> ScharrAveraging(const Texture &input);
    
    Ptr<Texture> Differentiation(const Texture &input);
    
    void PrepareStencilTest();
    
private:
    
    Ptr<Program> canny, scharr, diffCanny;
};

inline CannyFilter::CannyFilter() : base("Canny")
{

}

inline void CannyFilter::LoadShaderPrograms()
{
    canny     = LoadScreenSpaceProgram("Canny");
    scharr    = LoadScreenSpaceProgram("Sobel1");
    diffCanny = LoadScreenSpaceProgram("CannySobel2");
}

inline Ptr<Texture> CannyFilter::Gradients(const Texture &input)
{
    return Differentiation( *ScharrAveraging(input) );
}