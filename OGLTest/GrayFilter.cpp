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
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(input);
    return Render("Grayscale");
}