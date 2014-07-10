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
    
    const String& GetName() const;
    
    bool IsInitialized() const;
    
    void SetValue(bool value);
    
    void SetValue(int value);
    
    void SetValue(float value);
    
    void SetValue(const cv::Vec2i &value);
    
    void SetValue(const Vector2 &value);
    
    void SetValue(const Vector3 &value);
    
    void SetValue(const cv::Vec4f &value);
    
    void SetValue(const Texture &texture);
    
    void AssertLocation() const;
    
private:
    
    String name;
    GLint  location;
    GLenum type;
    int index; // Only used for texture/sampler indexing
    bool initialized;
};

inline Uniform::Uniform() : Uniform("", -1, GL_ZERO)
{
    
}

inline Uniform::Uniform(const String& name, GLint location, GLenum type, int index) : name(name), location(location), type(type), index(index), initialized(false)
{
    
}

inline const String& Uniform::GetName() const
{
    return name;
}

inline bool Uniform::IsInitialized() const
{
    return initialized;
}

inline void Uniform::SetValue(bool value)
{
    AssertLocation();
    initialized = true;
    glUniform1i(location, value);
}

inline void Uniform::SetValue(int value)
{
    AssertLocation();
    initialized = true;
    glUniform1i(location, value);
}

inline void Uniform::SetValue(float value)
{
    AssertLocation();
    initialized = true;
    glUniform1f(location, value);
}

inline void Uniform::SetValue(const cv::Vec2i &value)
{
    AssertLocation();
    initialized = true;
    glUniform2i(location, value[0], value[1]);
}

inline void Uniform::SetValue(const Vector2 &value)
{
    AssertLocation();
    initialized = true;
    glUniform2f(location, value[0], value[1]);
}

inline void Uniform::SetValue(const Vector3 &value)
{
    AssertLocation();
    initialized = true;
    glUniform3f(location, value[0], value[1], value[2]);
}

inline void Uniform::SetValue(const cv::Vec4f &value)
{
    AssertLocation();
    initialized = true;
    glUniform4f(location, value[0], value[1], value[2], value[3]);
}

inline void Uniform::AssertLocation() const
{
    if (location == -1)
        throw std::runtime_error("Error: Setting value of inactive uniform");
}