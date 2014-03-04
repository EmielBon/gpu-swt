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
#include <GL/glew.h>

void GraphicsDevice::DrawPrimitives(PrimitiveType type)
{
    // bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer->GetHandle());
    // draw the VAO
    glDrawElements(type, IndexBuffer->Count(), GL_UNSIGNED_BYTE, (GLvoid*)NULL);
}