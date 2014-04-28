//
//  GrayFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GrayFilter.h"

Ptr<Texture> GrayFilter::PerformSteps()
{
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(*Input);
    return Render("Grayscale");
}