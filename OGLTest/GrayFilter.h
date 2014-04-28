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
    
    GrayFilter(Ptr<Texture> input = nullptr);
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps();

private:
    
    Ptr<Program> grayscale;
};

inline GrayFilter::GrayFilter(Ptr<Texture> input) : base("Grayscale", input)
{
    
}

inline void GrayFilter::LoadShaderPrograms()
{
    grayscale = LoadScreenSpaceProgram("Grayscale");
}