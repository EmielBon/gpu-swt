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
    
    void PrepareBoundingBoxCalculation();
    
    void PrepareComponentCounting();
    
    void PreparePerPixelVertices();
    
    void BoundingBoxes(Ptr<Texture> input, Ptr<Texture> output, bool clear);
    
    void FilterInvalidComponents(Ptr<Texture> input, Ptr<Texture> output);
    
    void CountComponents(Ptr<Texture> input, Ptr<Texture> output);
    
    void PrepareStencilRouting(int componentCount);
    
    void StencilRouting(Ptr<Texture> input, float N, Ptr<Texture> output);
    
    void ExtractBoundingBoxes(int N);
    
    void PerformSteps(Ptr<Texture> output);
   
public:
    
    List<BoundingBox> ExtractedBoundingBoxes;
    
private:
    
    Ptr<Filter> grayFilter;
    Ptr<SWTFilter> swtFilter;
    Ptr<ConnectedComponentsFilter> connectedComponentsFilter;
    Ptr<Texture> gray, stencil;
    Ptr<Program> boundingBoxes, filterInvalidComponents, countComponents, stencilRouting;
    // todo: for debug purposes
    Ptr<Program> vertexTexture;
    Ptr<VertexBuffer> perPixelVertices;
};

inline TextRegionsFilter::TextRegionsFilter(Ptr<Texture> input)
    : base("TextRegions", input)
{
    
}