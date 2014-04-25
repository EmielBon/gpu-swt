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
    
    GrayFilter();
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps(const Texture &input);

private:
    
    Ptr<Program> grayscale;
};

inline GrayFilter::GrayFilter() : base("Grayscale")
{
    
}

inline void GrayFilter::LoadShaderPrograms()
{
    grayscale = LoadScreenSpaceProgram("Grayscale");
}