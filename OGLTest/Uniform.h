//
//  Uniform.h
//  OGLTest
//
//  Created by Emiel Bon on 07-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class Uniform
{
public:
    
    Uniform();
    
    Uniform(const String& name, GLint location, GLenum type, int index = 0);
    
    void SetValue(bool value);
    
    void SetValue(int value);
    
    void SetValue(float value);
    
    void SetValue(const cv::Vec2i &value);
    
    void SetValue(const Vector2 &value);
    
    void SetValue(const Texture &texture);
    
private:
    
    String name;
    GLint  location;
    GLenum type;
    int index; // Only used for texture/sampler indexing
};

inline Uniform::Uniform() : Uniform("", -1, GL_ZERO)
{
    
}

inline Uniform::Uniform(const String& name, GLint location, GLenum type, int index) : name(name), location(location), type(type), index(index)
{
    
}

inline void Uniform::SetValue(bool value)
{
    glUniform1i(location, value);
}

inline void Uniform::SetValue(int value)
{
    glUniform1i(location, value);
}

inline void Uniform::SetValue(float value)
{
    glUniform1f(location, value);
}

inline void Uniform::SetValue(const cv::Vec2i &value)
{
    glUniform2i(location, value[0], value[1]);
}

inline void Uniform::SetValue(const Vector2 &value)
{
    glUniform2f(location, value[0], value[1]);
}