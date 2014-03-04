//
//  Uniform.cpp
//  OGLTest
//
//  Created by Emiel Bon on 07-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Uniform.h"
#include "Texture.h"

void Uniform::SetValue(const Texture &texture)
{
    // todo: couple GL_TEXTURE[n] and SetValue(n)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
    SetValue(0);
}