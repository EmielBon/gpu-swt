//
//  VertexPositionTexture.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "VertexPositionTexture.h"
#include "VertexDeclaration.h"

const VertexDeclaration& VertexPositionTexture::VertexDeclaration()
{
    static ::VertexDeclaration *vertexDeclaration = NULL;
    
    if (!vertexDeclaration)
    {
        vertexDeclaration = new ::VertexDeclaration();
        vertexDeclaration->AddElement( ::VertexDeclarationElement("Position", 3, GL_FLOAT, sizeof(GLfloat)) );
        vertexDeclaration->AddElement( ::VertexDeclarationElement("TexCoord", 2, GL_FLOAT, sizeof(GLfloat)) );
    }
    
    return *vertexDeclaration;
}