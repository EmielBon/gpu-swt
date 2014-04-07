//
//  DepthBuffer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 18-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(Type type, int width, int height)
{
    glGenRenderbuffers(1, &bufferId);
    
    Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)type, width, height);
    Unbind();
}