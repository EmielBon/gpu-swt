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

DrawableRect::DrawableRect(float x1, float y1, float x2, float y2)
{
    List<VertexPositionTexture> vertices(4);
    
    vertices[0].Position = Vector3(x1, -y1, 0);
    vertices[0].TexCoord = Vector2(0, 0);
    vertices[1].Position = Vector3(x2, -y1, 0);
    vertices[1].TexCoord = Vector2(1, 0);
    vertices[2].Position = Vector3(x1, -y2, 0);
    vertices[2].TexCoord = Vector2(0, 1);
    vertices[3].Position = Vector3(x2, -y2, 0);
    vertices[3].TexCoord = Vector2(1, 1);
    
    List<GLubyte> indices = { 0, 1, 2, 1, 2, 3 };
    
    VertexBuffer = New<::VertexBuffer>();
    VertexBuffer->SetData(vertices);
    IndexBuffer = New<::IndexBuffer>();
    IndexBuffer->SetData(indices);
}