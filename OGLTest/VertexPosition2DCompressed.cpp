//
//  VertexPositionCompressed.cpp
//  OGLTest
//
//  Created by Emiel Bon on 19-06-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "VertexPosition2DCompressed.h"
#include "VertexDeclaration.h"

const VertexDeclaration& VertexPosition2DCompressed::VertexDeclaration()
{
    static ::VertexDeclaration *vertexDeclaration = NULL;
    
    if (!vertexDeclaration)
    {
        vertexDeclaration = new ::VertexDeclaration();
        vertexDeclaration->AddElement( ::VertexDeclarationElement("Position", 1, GL_FLOAT, sizeof(GLfloat)) );
    }
    
    return *vertexDeclaration;
}