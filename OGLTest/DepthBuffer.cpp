//
//  DepthBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "DepthBuffer.h"

DepthBuffer::DepthBuffer(int width, int height)
{
    glGenRenderbuffers(1, &bufferId);
    
    Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    Unbind();
}