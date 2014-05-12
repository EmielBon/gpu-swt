//
//  GrayFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GrayFilter.h"

void GrayFilter::PerformSteps(Ptr<Texture> output)
{
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(*Input);
    RenderToTexture(output);
}