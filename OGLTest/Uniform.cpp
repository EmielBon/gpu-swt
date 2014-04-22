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
    // resolved: couple GL_TEXTURE[n] and SetValue(n)
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
    //glEnable(GL_TEXTURE_2D);
    SetValue(index);
}