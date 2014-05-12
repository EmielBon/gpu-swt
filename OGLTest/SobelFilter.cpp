//
//  SobelFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SobelFilter.h"
#include "Texture.h"

void SobelFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(1);
    ScharrAveraging(*Input, ColorBuffers[0]);
    Differentiation(*ColorBuffers[0], output);
}

void SobelFilter::ScharrAveraging(const Texture &input, Ptr<Texture> output)
{
    scharr->Use();
    scharr->Uniforms["Texture"].SetValue(input);
    RenderToTexture(output);
}

void SobelFilter::Differentiation(const Texture &input, Ptr<Texture> output)
{
    diff->Use();
    diff->Uniforms["Texture"].SetValue(input);
    RenderToTexture(output);
}