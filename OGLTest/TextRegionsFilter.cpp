//
//  TextRegionsFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 23-05-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "TextRegionsFilter.h"
#include "GrayFilter.h"
#include "SWTFilter.h"
#include "ConnectedComponentsFilter.h"
#include "Texture.h"
#include "FrameBuffer.h"

void TextRegionsFilter::LoadShaderPrograms()
{
    grayFilter  = New<GrayFilter>(Input);
    swtFilter   = New<SWTFilter>();
    connectedComponentsFilter = New<ConnectedComponentsFilter>();
    
    grayFilter->DoLoadShaderPrograms();
    swtFilter->DoLoadShaderPrograms();
    connectedComponentsFilter->DoLoadShaderPrograms();
}

void TextRegionsFilter::Initialize()
{
    gray = FrameBuffer::GetCurrentlyBound()->ColorAttachment0->GetEmptyClone();
    ApplyFilter(*grayFilter, gray);
    swtFilter->Input = gray;
    
    ReserveColorBuffers(1);
}

void TextRegionsFilter::PerformSteps(Ptr<Texture> output)
{
    swtFilter->GradientDirection = GradientDirection;
    ApplyFilter(*swtFilter, ColorBuffers[0]);
    swtFilter->GradientDirection = GradientDirection::Against;
    connectedComponentsFilter->Input = ColorBuffers[0];
    ApplyFilter(*connectedComponentsFilter, output);
}