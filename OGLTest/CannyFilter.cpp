//
//  CannyFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "CannyFilter.h"
#include "Texture.h"
#include "GaussianFilter.h"
#include "FrameBuffer.h"
#include "GraphicsDevice.h"
#include "RenderWindow.h"

void CannyFilter::LoadShaderPrograms()
{
    gaussian = New<GaussianFilter>();
    gaussian->DoLoadShaderPrograms();
    
    histogram = LoadProgram("Histogram", "Value");
    canny     = LoadScreenSpaceProgram("Canny");
    scharr    = LoadScreenSpaceProgram("Sobel1");
    diffCanny = LoadScreenSpaceProgram("CannySobel2");
}

void CannyFilter::Initialize()
{
    glClearColor(0, 0, 0, 0);
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
    ReserveColorBuffers(2);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
    GraphicsDevice::SetBuffers(PerPixelVertices, nullptr);
    histogram->Use();
    histogram->Uniforms["Texture"].SetValue(*Input);
    RenderToTexture(ColorBuffers[1], PrimitiveType::Points, GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    GraphicsDevice::UseDefaultBuffers();
    
    // todo: maybe the histogram generation can be done in the gray filter. Slower because of the per pixel vertices, but probably faster than a whole extra pass
    auto pixels = FrameBuffer::GetCurrentlyBound()->ReadPixels<float>(0, 0, 255, 1, GL_RED, GL_FLOAT);
    // Estimate median from frequencies
    float percentile = 0;
    int i;
    for(i = 0; i < 255 && percentile < 0.5; ++i)
        percentile += pixels[i] / (Input->GetWidth() * Input->GetHeight());
    float median = i / 255.0f;
    
    gaussian->Input = Input;
    ApplyFilter(*gaussian, ColorBuffers[0]);
    
    ScharrAveraging(*ColorBuffers[0], output);
    Differentiation(*output, ColorBuffers[0]);
    
    //glEnable(GL_STENCIL_TEST);
    // todo: why / 2 ? Would it benefit from contrast stretch? Or should I use the 0.33rd and 0.66th percentile?? That would actually make a lot more sense...
    DetectEdges(*ColorBuffers[0], 0.33f/2 * median, 0.66f/2 * median, output); // Buffer0 contains gradients
    DEBUG_FB("Edges");
    //glDisable(GL_STENCIL_TEST);
}

void CannyFilter::DetectEdges(const Texture &gradients, float lowerThreshold, float upperThreshold, Ptr<Texture> output)
{
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(gradients);
    canny->Uniforms["LowerThreshold"].SetValue(lowerThreshold);
    canny->Uniforms["UpperThreshold"].SetValue(upperThreshold);
    // Make sure the color buffer is empty because Canny discards non-edge pixels
    RenderToTexture(output, PrimitiveType::Unspecified, GL_COLOR_BUFFER_BIT/* | GL_STENCIL_BUFFER_BIT*/);
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