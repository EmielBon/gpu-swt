//
//  DrawableRect.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "DrawableRect.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexPosition.h"

DrawableRect::DrawableRect(float x1, float y1, float x2, float y2)
{
    List<VertexPosition> vertices(4);
    
    vertices[0].Position = Vector3(x1, y1, 1);
    vertices[1].Position = Vector3(x2, y1, 1);
    vertices[2].Position = Vector3(x1, y2, 1);
    vertices[3].Position = Vector3(x2, y2, 1);
    
    List<GLuint> indices = { 0, 1, 2, 1, 2, 3 };
    
    VertexBuffer = New<::VertexBuffer>();
    VertexBuffer->SetData(vertices);
    IndexBuffer = New<::IndexBuffer>();
    IndexBuffer->SetData(indices);
}