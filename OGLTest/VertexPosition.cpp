//
//  VertexPosition.cpp
//  OGLTest
//
//  Created by Emiel Bon on 17-06-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "VertexPosition.h"
#include "VertexDeclaration.h"

const VertexDeclaration& VertexPosition::VertexDeclaration()
{
    static ::VertexDeclaration *vertexDeclaration = NULL;
    
    if (!vertexDeclaration)
    {
        vertexDeclaration = new ::VertexDeclaration();
        vertexDeclaration->AddElement( ::VertexDeclarationElement("Position", 3, GL_FLOAT, sizeof(GLfloat)) );
    }
    
    return *vertexDeclaration;
}
