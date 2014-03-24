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
    
    Uniform(const String& name, GLint location, GLenum type);
    
    void SetValue(GLint value);
    
    void SetValue(GLfloat value);
    
    void SetValue(const cv::Vec2i &value);
    
    void SetValue(const Vector2 &value);
    
    void SetValue(const Texture &texture);
    
private:
    
    String name;
    GLint  location;
    GLenum type;
};

inline Uniform::Uniform() : location(-1), type(GL_ZERO)
{
    
}

inline Uniform::Uniform(const String& name, GLint location, GLenum type) : name(name),location(location), type(type)
{
    
}

inline void Uniform::SetValue(GLint value)
{
    glUniform1i(location, value);
}

inline void Uniform::SetValue(GLfloat value)
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