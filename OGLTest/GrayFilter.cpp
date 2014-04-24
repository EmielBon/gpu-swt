//
//  GrayFilter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GrayFilter.h"

GrayFilter::GrayFilter(GraphicsDevice *device, const Texture &input) : base(device)
{
    
}

void GrayFilter::LoadShaderPrograms()
{
    AddScreenSpaceProgram("Grayscale");
}