//
//  StrokeWidthTransformProgram.h
//  OGLTest
//
//  Created by Emiel Bon on 10-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class StrokeWidthTransformProgram
{
public:
    
    StrokeWidthTransformProgram();
    
    void Use();
    
public:
    
    Ptr<Texture> Edges;
    Ptr<Texture> Gradients;
    GradientDirection Direction;
};
