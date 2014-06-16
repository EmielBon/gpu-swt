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
    
    List<BoundingBox> GetExtractedBoundingBoxes() const;
    
public:
    
    ::GradientDirection GradientDirection;
    
private:
    
    Ptr<Filter> grayFilter;
    Ptr<SWTFilter> swtFilter;
    Ptr<ConnectedComponentsFilter> connectedComponentsFilter;
    Ptr<Texture> gray;
    Ptr<Program> vertexTexture;
};

inline TextRegionsFilter::TextRegionsFilter(Ptr<Texture> input)
    : base("TextRegions", input), GradientDirection(GradientDirection::Unspecified)
{
    
}