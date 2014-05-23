//
//  CannyFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "CannyFilter.h"
#include "Texture.h"

void CannyFilter::LoadShaderPrograms()
{
    canny     = LoadScreenSpaceProgram("Canny");
    scharr    = LoadScreenSpaceProgram("Sobel1");
    diffCanny = LoadScreenSpaceProgram("CannySobel2");
}

void CannyFilter::Initialize()
{
    glClearColor(0, 0, 0, 1);
    PrepareStencilTest();
}

void CannyFilter::PrepareStencilTest()
{
    glClearStencil(0);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
}

void CannyFilter::PerformSteps(Ptr<Texture> output)
{
    ReserveColorBuffers(1);
    
    ScharrAveraging(*Input, output);
    Differentiation(*output, ColorBuffers[0]);
    
    SetColorAttachment(output);
    glEnable(GL_STENCIL_TEST);
    // Make sure the color buffer is empty because Canny discards non-edge pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    DetectEdges(*ColorBuffers[0]); // Buffer0 contains gradients
    glDisable(GL_STENCIL_TEST);
}

void CannyFilter::DetectEdges(const Texture &gradients)
{
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(gradients);
    Render();
}

void CannyFilter::ScharrAveraging(const Texture &input, Ptr<Texture> output)
{
    scharr->Use();
    scharr->Uniforms["Texture"].SetValue(input);
    RenderToTexture(output);
}

void CannyFilter::Differentiation(const Texture &input, Ptr<Texture> output)
{
    diffCanny->Use();
    diffCanny->Uniforms["Texture"].SetValue(input);
    RenderToTexture(output);
}