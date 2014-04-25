//
//  SWTFilter.h
//  OGLTest
//
//  Created by Emiel Bon on 25-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Filter.h"
#include "SobelFilter.h"
#include "CannyFilter.h"

class SWTFilter : public Filter
{
private:
    
    using base = Filter;
    
public:
    
    SWTFilter();
    
    void LoadShaderPrograms();
    
    Ptr<Texture> PerformSteps(const Texture &input);
    
private:
    
    Ptr<Texture> CastRays(const Texture &gradients, bool darkOnLight);
    
    Ptr<Texture> WriteRayValues(const Texture &values, const Texture &gradients, const Texture &lineLengths, bool darkOnLight);
    
    Ptr<Texture> AverageRayValues(const Texture &values, const Texture &gradients, bool darkOnLight);
    
    Ptr<Texture> ScaleResult(const Texture &input, float scale);
    
    void PrepareStencilTest();
    
    void PrepareDepthTest();
    
    void PrepareRayLines(const Texture &values);
    
private:
    
    Ptr<Program> cast, write, avg, scale;
    SobelFilter sobel;
    CannyFilter canny;
    
    Ptr<VertexBuffer> linesVertices;
    Ptr<IndexBuffer>  linesIndices;
};

inline SWTFilter::SWTFilter() : base("StrokeWidthTransform")
{
    
}

inline void SWTFilter::LoadShaderPrograms()
{
    cast  = LoadScreenSpaceProgram("StrokeWidthTransform1");
    write =            LoadProgram("StrokeWidthTransform2");
    avg   = LoadScreenSpaceProgram("StrokeWidthTransform3");
    scale = LoadScreenSpaceProgram("ScaleColor");
}