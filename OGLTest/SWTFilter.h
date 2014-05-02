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
    
    Ptr<Texture> PerformSteps();
    
private:
    
    void PrepareEdgeOnlyStencil();
    
    void PrepareMaximizingDepthTest();
    
    void PrepareRayLines(const Texture &values);
    
    Ptr<Texture> CastRays(bool darkOnLight);
    
    Ptr<Texture> WriteRayValues(const Texture &values, const Texture &lineLengths, bool darkOnLight);
    
    Ptr<Texture> AverageRayValues(const Texture &values, bool darkOnLight);
    
    Ptr<Texture> ScaleResult(const Texture &input, float scaleFactor);
    
private:
    
    Ptr<Program> cast, write, avg, scale;
    Ptr<Filter> sobel, gaussian, canny;
    Ptr<Texture> edges, gradients;
    Ptr<VertexBuffer> linesVertices;
    
public:
    
    ::GradientDirection GradientDirection;
};

inline SWTFilter::SWTFilter(Ptr<Texture> input)
    : base("StrokeWidthTransform", input), GradientDirection(GradientDirection::Unspecified)
{
    
}