//
//  StencilBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "StencilBuffer.h"

StencilBuffer::StencilBuffer(int width, int height)
{
    glGenRenderbuffers(1, &bufferId);
    
    Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    Unbind();
}