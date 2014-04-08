//
//  GraphicsDevice.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "GraphicsDevice.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

void GraphicsDevice::DrawPrimitives(PrimitiveType type)
{
    // bind the index buffer
    IndexBuffer->Bind();
    // draw the VAO
    glDrawElements((GLenum)type, IndexBuffer->Count(), GL_UNSIGNED_SHORT, (GLvoid*)NULL);
}