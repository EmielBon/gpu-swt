//
//  FragmentShader.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Shader.h"

class FragmentShader : public Shader
{
private:
    
    typedef Shader base;
    
public:
    
    FragmentShader(const String &name, const String &sourceText);
};

inline FragmentShader::FragmentShader(const String &name, const String &sourceText)
    : base(name, sourceText, GL_FRAGMENT_SHADER)
{
    
}