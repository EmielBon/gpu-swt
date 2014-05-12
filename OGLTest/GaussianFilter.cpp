//
//  GaussianFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GaussianFilter.h"
#include "Texture.h"

void GaussianFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(1);
    HorizontalPass(*Input, ColorBuffers[0]);
    VerticalPass(*ColorBuffers[0], output);
}

void GaussianFilter::HorizontalPass(const Texture &input, Ptr<Texture> output)
{
    SetColorAttachment(output);
    hor->Use();
    hor->Uniforms["Texture"].SetValue(input);
    Render();
}

void GaussianFilter::VerticalPass(const Texture &input, Ptr<Texture> output)
{
    SetColorAttachment(output);
    ver->Use();
    ver->Uniforms["Texture"].SetValue(input);
    Render();
}