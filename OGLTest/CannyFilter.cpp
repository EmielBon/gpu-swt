//
//  CannyFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "CannyFilter.h"

Ptr<Texture> CannyFilter::PerformSteps(const Texture &input)
{
    glClearColor(0, 0, 0, 1);
    PrepareStencilTest();
    
    auto gradients = Gradients(input);
    
    // Make sure the color buffer is empty because Canny discards non-edge pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    auto edges = DetectEdges(*gradients);
    glDisable(GL_STENCIL_TEST);
    
    return edges;
}

Ptr<Texture> CannyFilter::DetectEdges(const Texture &gradients)
{
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(gradients);
    return Render("Edges (Canny)");
}

Ptr<Texture> CannyFilter::ScharrAveraging(const Texture &input)
{
    scharr->Use();
    scharr->Uniforms["Texture"].SetValue(input);
    return Render("CannySobel1");
}

Ptr<Texture> CannyFilter::Differentiation(const Texture &input)
{
    diffCanny->Use();
    diffCanny->Uniforms["Texture"].SetValue(input);
    return Render("CannySobel2");
}

void CannyFilter::PrepareStencilTest()
{
    glClearStencil(0);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
}