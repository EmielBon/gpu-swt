//
//  ConnectedComponentsFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 28-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class ConnectedComponentsFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    ConnectedComponentsFilter(Ptr<Texture> input = nullptr);
    
    void Initialize();
    
protected:
    
    void PrepareMaximizingDepthTest();
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps();
    
    Ptr<Texture> Encode();
    
    Ptr<Texture> VerticalRuns(Ptr<Texture> input);
    
    void GatherNeighbor(Ptr<Texture> input, Ptr<Texture> dest);
    
    void UpdateColumn(Ptr<Texture> input, Ptr<Texture> dest);
    
    void ScatterBack(Ptr<Texture> input, Ptr<Texture> dest);
    
private:
    
    Ptr<Program> encode, verticalRun, gatherNeighbor, updateColumn, scatterBack;
};

inline ConnectedComponentsFilter::ConnectedComponentsFilter(Ptr<Texture> input)
    : base("Connected Components", input)
{
    
}