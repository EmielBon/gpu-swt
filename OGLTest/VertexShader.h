//
//  VertexShader.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Shader.h"

class VertexShader : public Shader
{
private:
    
    typedef Shader base;
    
public:
    
    VertexShader(const String &sourceText);
};

inline VertexShader::VertexShader(const String &sourceText)
    : base(sourceText, GL_VERTEX_SHADER)
{
    
}
