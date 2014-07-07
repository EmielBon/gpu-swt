//
//  SWTFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 25-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"

class SWTFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    SWTFilter(Ptr<Texture> input = nullptr);
    
    void Initialize();
    
protected:
    
    void LoadShaderPrograms();
    
    void PerformSteps(Ptr<Texture> output);
    
private:
    
    void PrepareEdgeOnlyStencil();
    
    void PrepareRayLines(const Texture &values);
    
    void CastRays(bool darkOnLight, Ptr<Texture> output);
    
    void WriteRayValues(const Texture &oppositePositions, Ptr<Texture> output);
    
    void WriteRayValues2(const Texture &oppositePositions, Ptr<Texture> output);
    
    void AverageRayValues(const Texture &oppositePositions, const Texture &values, Ptr<Texture> output);
    
    void WriteAverageRayValues(const Texture &oppositePositions, const Texture &averageValues, Ptr<Texture> output);
    
    void WriteAverageRayValues2(const Texture &oppositePositions, const Texture &averageValues, Ptr<Texture> output);
    
    void ScaleResult(const Texture &input, float scaleFactor, Ptr<Texture> output);
    
private:
    
    Ptr<Program> cast, write, avg, writeAvg, scale;
    Ptr<Filter> sobel;
    Ptr<CannyFilter> canny;
    Ptr<Texture> edges, gradients;
    Ptr<VertexBuffer> linesVertices;
    
public:
    
    ::GradientDirection GradientDirection;
};

inline SWTFilter::SWTFilter(Ptr<Texture> input)
    : base("StrokeWidthTransform", input), GradientDirection(GradientDirection::Unspecified)
{
    
}