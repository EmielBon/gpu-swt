//
//  SWTParameters.h
//  OGLTest
//
//  Created by Emiel Bon on 10-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include <math.h>
#include <float.h>

//#define USE_FAKE_MEDIAN
//#define SHARPEN_INPUT
//#define EQUALIZE_HISTOGRAM
#define INCLUDE_SECOND_EDGE_PIXEL_IN_RAY

const float MaxOppositeEdgeGradientDifference = M_PI / 2; // Paper says Pi/6, but this does not work well
const float MaxRayLength = 50; // todo: make image-size dependent
const float RayStepPrecision = 0.2;
const float MinAspectRatio = 0.2; // Paper says 0.1
const float MaxAspectRatio = 2; // Paper says 10, but this allows for very short, broad shapes which are generally not letters
const float MaxSWTRatio = 3.0;
const float MaxComponentSWTRatio = 2.0;
const float MaxComponentHeightRatio = 2.0; // Paper says 2, but for Chep 1.1 is better
const float MaxComponentDistanceRatio = 3.0;
const float MinOccupancy = 0.2;
const float MaxOccupancy = 0.9;
const float MaxColorDifference = 0.1;
const float MaxNullAngleDifference = M_PI / 16; // Not in paper
const float MaxVarianceToMeanRatio = 1.5; // Paper says 2.0, but this (combined with general inaccuracies in traced rays) eliminates letters in some fonts
const int   MinChainLength = 7;
const int   MaxChainLength = 12;