//
//  ConnectedComponentsFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 28-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"
#include "BoundingBox.h"

class ConnectedComponentsFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    ConnectedComponentsFilter(Ptr<Texture> input = nullptr);
    
    void Initialize();
    
protected:
    
    void PrepareMaximizingDepthTest();
    
    void PrepareColumnVertices();
    
    void PrepareLineIndices();
    
    void PrepareVerticalRuns();
    
    void PrepareBoundingBoxCalculation();
    
    void PrepareComponentCounting();
    
    void PreparePerPixelVertices();
    
    void LoadShaderPrograms();
    
    void PerformSteps(Ptr<Texture> output);
    
    void Encode(Ptr<Texture> input, Ptr<Texture> output);
    
    // This uses the provided input texture as a rendertarget for intermediate steps, overwriting its original values
    void VerticalRuns(Ptr<Texture> input, Ptr<Texture> output);
    
    void GatherNeighbor(Ptr<Texture> input, int column, Ptr<Texture> output);
    
    void UpdateColumn(Ptr<Texture> input, int column, Ptr<Texture> output);
    
    void ScatterBack(Ptr<Texture> input, int column, Ptr<Texture> output);
    
    void UpdateRoots(Ptr<Texture> input, Ptr<Texture> output);
    
    void UpdateChildren(Ptr<Texture> input, Ptr<Texture> output);
    
    void BoundingBoxes(Ptr<Texture> input, Ptr<Texture> output);
    
    void FilterInvalidComponents(Ptr<Texture> input, Ptr<Texture> output);
    
    void CountComponents(Ptr<Texture> input, Ptr<Texture> output);
    
    void ExtractBoundingBoxes();
    
    void Decode(Ptr<Texture> input, Ptr<Texture> output);
    
    void Copy(Ptr<Texture> texture, Ptr<Texture> output);
    
public:
    
    List<BoundingBox> ExtractedBoundingBoxes;
    
private:
    
    Ptr<Program> encode, verticalRun, gatherNeighbor, updateColumn, scatterBack, updateRoots, updateChildren, boundingBoxes, filterInvalidComponents, countComponents;
    Ptr<Program> normal, decode;
    Ptr<VertexBuffer> columnVertices;
    Ptr<IndexBuffer>  lineIndices;
    Ptr<VertexBuffer> perPixelVertices;
};

inline ConnectedComponentsFilter::ConnectedComponentsFilter(Ptr<Texture> input)
    : base("Connected Components", input)
{
    
}