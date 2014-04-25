//
//  CannyFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "CannyFilter.h"

Ptr<Texture> CannyFilter::DetectEdges(const Texture &gradients)
{
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
    
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(gradients);
    auto edges = Render("Edges (Canny)");
    
    glDisable(GL_STENCIL_TEST);
    
    return edges;
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

