//
//  SWTHelperGPU.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTHelperGPU.h"

#include "FrameBuffer.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "ContentLoader.h"
#include "Program.h"
#include "Texture.h"
#include "DrawableRect.h"
#include "RenderWindow.h"
#include "BoundingBox.h"
#include "ImgProc.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GLError.h"
#include "Profiling.h"
#include "GrayFilter.h"
#include "SobelFilter.h"
#include "GaussianFilter.h"
#include "CannyFilter.h"
#include "SWTFilter.h"
#include "ConnectedComponentsFilter.h"
#include "TextureUtil.h"

Ptr<Texture> ApplyPass(Ptr<Filter> filter, Ptr<Texture> input = nullptr);

TimeSpan renderTime;
TimeSpan copyTime;
TimeSpan compileTime;

List<BoundingBox> SWTHelperGPU::StrokeWidthTransform(const cv::Mat &input)
{
    auto t = now();
    
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    //glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    //glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);
    
    glDisable(GL_DITHER);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glPixelZoom(1.0,1.0);
    
    int width  = input.size().width;
    int height = input.size().height;
    
    auto grayFilter  = New<GrayFilter>();
    auto swtFilter   = New<SWTFilter>();
    auto connectedComponentsFilter = New<ConnectedComponentsFilter>();
    
    grayFilter->DoLoadShaderPrograms();
    swtFilter->DoLoadShaderPrograms();
    connectedComponentsFilter->DoLoadShaderPrograms();
    
    // Create a Texture from the input
    Ptr<Texture> texture = textureFromImage<cv::Vec3f>(input);
    
    // Create the framebuffer attachments
    Ptr<Texture>      colorf       = New<Texture     >(width, height, GL_RGBA, GL_FLOAT, GL_NEAREST);
    Ptr<RenderBuffer> depthStencil = New<RenderBuffer>(width, height, RenderBuffer::Type::DepthStencil);
    
    // Create and setup framebuffer
    FrameBuffer frameBuffer;
    frameBuffer.Attach(colorf);
    
    // Create a full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    GraphicsDevice::SetDefaultBuffers(rect.VertexBuffer, rect.IndexBuffer);
    GraphicsDevice::UseDefaultBuffers();
    
    frameBuffer.Bind();

    auto gray        = ApplyPass(grayFilter, texture);
    grayFilter.reset();
    swtFilter->Input = gray;
    swtFilter->GradientDirection = GradientDirection::With;
    auto swt1        = ApplyPass(swtFilter, gray);
    swtFilter->GradientDirection = GradientDirection::Against;
    auto swt2        = ApplyPass(swtFilter, gray);
    gray.reset();
    swtFilter.reset();
    auto components1 = ApplyPass(connectedComponentsFilter, swt2);
    swt1.reset();
    components1.reset();
    
    glFinish();
    auto totalTime = now() - t;
    
    auto misc = totalTime - renderTime - copyTime - compileTime;
    
    printf("%s: T(%.1fms) R(%.1fms=%.1f%%) Cpy(%.1fms=%.1f%%) Cpl(%.1fms=%.1f%%) M(%.1fms=%.1f%%)\n", "Total",
           GetTimeMsec(totalTime),
           GetTimeMsec(renderTime),
           renderTime * 100.0f / totalTime,
           GetTimeMsec(copyTime),
           copyTime * 100.0f / totalTime,
           GetTimeMsec(compileTime),
           compileTime * 100.0f / totalTime,
           GetTimeMsec(misc),
           misc * 100.0f / totalTime
    );

    printf("Textures: Active %i Peak %i\n", Texture::ActiveTextureCount, Texture::PeakTextureCount);
    
    return List<BoundingBox>();
}

Ptr<Texture> ApplyPass(Ptr<Filter> filter, Ptr<Texture> input)
{
    auto output = FrameBuffer::GetCurrentlyBound()->ColorAttachment0->GetEmptyClone();
    
    if (input)
        filter->Input = input;
    filter->Apply(output);
    renderTime  += filter->RenderTime;
    copyTime    += filter->CopyTime;
    compileTime += filter->CompileTime;
    
    RenderWindow::Instance().AddFrameBufferSnapshot(filter->Name);
    return output;
}