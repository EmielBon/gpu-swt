//
//  SobelFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SobelFilter.h"

Ptr<Texture> SobelFilter::ScharrAveraging(const Texture &input)
{
    scharr->Use();
    scharr->Uniforms["Texture"].SetValue(input);
    return Render("Sobel1");
}

Ptr<Texture> SobelFilter::Differentiation(const Texture &input)
{
    diff->Use();
    diff->Uniforms["Texture"].SetValue(input);
    return Render("Sobel2");
}