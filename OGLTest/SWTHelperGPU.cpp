//
//  SWTHelperGPU.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTHelperGPU.h"
#include "FrameBuffer.h"
#include "ContentLoader.h"
#include "Texture.h"
#include "DrawableRect.h"
#include "RenderWindow.h"
#include "BoundingBox.h"
#include "Profiling.h"
#include "TextureUtil.h"
#include "TextRegionsFilter.h"
#include "VertexPosition.h"

TimeSpan renderTime, copyTime, compileTime;

List<BoundingBox> SWTHelperGPU::StrokeWidthTransform(const cv::Mat &input)
{
    glFinish();
    auto startTime = now();
    
    DisableIrrelvantState();
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    
    int width  = input.size().width;
    int height = input.size().height;
    // Create a Texture from the input
    Ptr<Texture> texture = textureFromImage<cv::Vec3f>(input);
    
    List< Ptr<Texture> > textures;
    
    /*for(int i = 0; i < 14; ++i)
        textures.push_back( New<Texture>(width, height, GL_RGBA, GL_FLOAT) );
    
    for(int i = 0; i < 14; ++i)
        textures[i].reset();
    */
    // Create the framebuffer attachments
    Ptr<Texture>      colorf       = New<Texture     >(width, height, GL_RGBA, GL_FLOAT);
    Ptr<RenderBuffer> depthStencil = New<RenderBuffer>(width, height, RenderBuffer::Type::DepthStencil);
    
    // Create and setup framebuffer
    FrameBuffer frameBuffer;
    frameBuffer.Attach(colorf);
    
    // Create a full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    GraphicsDevice::SetDefaultBuffers(rect.VertexBuffer, rect.IndexBuffer);
    GraphicsDevice::UseDefaultBuffers();
    
    frameBuffer.Bind();

    auto textRegionsFilter = New<TextRegionsFilter>(texture);
    textRegionsFilter->DoLoadShaderPrograms();
    
    glFinish();
    auto setupTime = now() - startTime;
    
    textRegionsFilter->GradientDirection = GradientDirection::With;
    ApplyPass(textRegionsFilter);
    textRegionsFilter->GradientDirection = GradientDirection::Against;
    ApplyPass(textRegionsFilter);
    
    glFinish();
    auto totalTime = now() - startTime;
    
    auto misc = totalTime - renderTime - setupTime;
    
    printf("\n");
    printf("Total time: %.1fms\n", GetTimeMsec(totalTime));
    printf("Setup time: %.1fms (%.1f%%)\n", GetTimeMsec(setupTime), setupTime * 100.0f / totalTime);
    printf("Render time: %.1fms (%.1f%%)\n", GetTimeMsec(renderTime), renderTime * 100.0f / totalTime);
    printf("Misc time: %.1fms (%.1f%%)\n", GetTimeMsec(misc), misc * 100.0f / totalTime);
    printf("Textures: Active %i Peak %i\n", Texture::ActiveTextureCount, Texture::PeakTextureCount);
    
    return textRegionsFilter->GetExtractedBoundingBoxes();
}

Ptr<Texture> SWTHelperGPU::ApplyPass(Ptr<Filter> filter, Ptr<Texture> input)
{
    auto output = FrameBuffer::GetCurrentlyBound()->ColorAttachment0->GetEmptyClone();
    
    if (input)
        filter->Input = input;
    filter->Apply(output);
    renderTime  += filter->RenderTime;
    compileTime += filter->CompileTime;
    
    DEBUG_FB(filter->Name);
    return output;
}

void SWTHelperGPU::DisableIrrelvantState()
{
    /*glDisable(GL_DITHER);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glPixelZoom(1.0,1.0);*/
    // todo: more?
    // todo: some state disabeling makes OpenGL Profiler crash T_T
}