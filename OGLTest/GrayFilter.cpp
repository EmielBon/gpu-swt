//
//  GrayFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GrayFilter.h"

Ptr<Texture> GrayFilter::PerformSteps(const Texture &input)
{
    Grayscale->Use();
    Grayscale->Uniforms["Texture"].SetValue(input);
    return Render("Grayscale");
}