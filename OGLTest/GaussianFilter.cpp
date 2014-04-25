//
//  GaussianFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GaussianFilter.h"

Ptr<Texture> GaussianFilter::HorizontalPass(const Texture &input)
{
    hor->Use();
    hor->Uniforms["Texture"].SetValue(input);
    return Render("Gaussian Blur (hor)");
}

Ptr<Texture> GaussianFilter::VerticalPass(const Texture &input)
{
    ver->Use();
    ver->Uniforms["Texture"].SetValue(input);
    return Render("Gaussian Blur (ver)");
}