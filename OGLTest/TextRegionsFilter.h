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
    
    void PrepareSummationOperations();
    
    void FindLetterCandidates(Ptr<Texture> input, GradientDirection gradientDirection, Ptr<Texture> output);
    
    void FilterInvalidComponents(Ptr<Texture> boundingBoxes, Ptr<Texture> averages, Ptr<Texture> occupancy, Ptr<Texture> variances, Ptr<Texture> output);
    
    void BoundingBoxes(Ptr<Texture> input, Ptr<Texture> output, bool clear);
    
    void CountComponents(Ptr<Texture> input, Ptr<Texture> output);
    
    void Occupancy(Ptr<Texture> input, Ptr<Texture> output, bool clear);
    
    void AverageColorAndSWT(Ptr<Texture> components, Ptr<Texture> occupancy, Ptr<Texture> inputImage, Ptr<Texture> strokeWidths, Ptr<Texture> output, bool clear);
    
    void Variance(Ptr<Texture> components, Ptr<Texture> occupancy, Ptr<Texture> strokeWidths, Ptr<Texture> averages, Ptr<Texture> output, bool clear);
    
    void PrepareStencilRouting(int N);
    
    void StencilRouting(Ptr<Texture> input, float N, Ptr<Texture> output);
    
    //void WriteIDs(Ptr<Texture> input, Ptr<Texture> output);
    
    void ExtractBoundingBoxes(int N, int count);
    
    void PerformSteps(Ptr<Texture> output);
    
public:
    
    List<BoundingBox> ExtractedBoundingBoxes;
    
private:
    
    Ptr<Filter> grayFilter;
    Ptr<SWTFilter> swtFilter;
    Ptr<ConnectedComponentsFilter> connectedComponentsFilter;
    Ptr<Texture> gray, stencil;
    Ptr<Program> boundingBoxes, filterInvalidComponents, countComponents, stencilRouting, calculateOccupancy, average, variance, writeIDs;
    // todo: for debug purposes
    Ptr<Program> vertexTexture;
};

inline TextRegionsFilter::TextRegionsFilter(Ptr<Texture> input)
    : base("TextRegions", input)
{
    
}