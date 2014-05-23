//
//  Filter.h
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "Program.h"

class Filter
{
protected:
    
    Filter(const String &name, Ptr<Texture> input = nullptr);
    
public:
    
    void Apply(Ptr<Texture> output);
    
    virtual void Initialize() { }
    
    void DoLoadShaderPrograms();
    
    void DoInitialize();
    
protected:
    
    virtual void PerformSteps(Ptr<Texture> output) = 0;
    
    virtual void LoadShaderPrograms() = 0;
    
    // Reserves auxilary color buffers for use. ColorBuffer[0] is always initialized to be the current ColorAttachment0 in the frame buffer
    void ReserveColorBuffers(int count);
    
    Ptr<Program> LoadScreenSpaceProgram(const String &name);
    
    Ptr<Program> LoadProgram(const String &name);
    
    Ptr<Program> LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource);
    
    // Apply a filter as part of this filter, aggregating the profiling information
    void ApplyFilter(Filter &filter, Ptr<Texture> output);
    
    void Render(PrimitiveType primitiveType = PrimitiveType::Unspecified, GLenum clearOptions = GL_NONE);

    void RenderToTexture(Ptr<Texture> destination, PrimitiveType primitiveType = PrimitiveType::Unspecified, GLenum clearOptions = GL_NONE);
    
    Ptr<Texture> GetColorAttachment();
    
    void SetColorAttachment(Ptr<Texture> colorAttachment);
    
    void PrintProfilingInfo() const;
    
private:
    
    bool shadersLoaded;
    bool initialized;
    
protected:
    
    List< Ptr<Texture> > ColorBuffers;
    
public:
    
    String Name;
    Ptr<Texture> Input;
    
    TimeSpan RenderTime, CompileTime, TotalTime;
};

inline Filter::Filter(const String &name, Ptr<Texture> input)
    : shadersLoaded(false), initialized(false), Name(name), Input(input), RenderTime(0), CompileTime(0), TotalTime(0)
{
    
}

inline Ptr<Program> Filter::LoadScreenSpaceProgram(const String &name)
{
    return LoadProgram("Trivial", name);
}

inline Ptr<Program> Filter::LoadProgram(const String &name)
{
    return LoadProgram(name, name);
}