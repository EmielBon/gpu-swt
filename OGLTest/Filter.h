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
    
    Filter(const String &name);
    
public:
    
    Ptr<Texture> Apply(const Texture &input);
    
protected:
    
    virtual Ptr<Texture> PerformSteps(const Texture &input) = 0;
    
    virtual void LoadShaderPrograms() = 0;
    
    Ptr<Program> LoadScreenSpaceProgram(const String &name);
    
    Ptr<Program> LoadProgram(const String &name);
    
    Ptr<Program> LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource);
    
    // Apply a filter as part of this filter, aggregating the profiling information
    Ptr<Texture> ApplyFilter(Filter &filter, const Texture &input);
    
    void StartAccumulatedRender() { accumulate = true; }
    
    Ptr<Texture> Render(const String &name);

    void EndAccumulatedRender() { accumulate = false; }
    
    void PrintProfilingInfo() const;
    
private:
    
    bool accumulate;
    unsigned long accumulated = 0;
    
public:
    
    String Name;
    unsigned long RenderTime;
    unsigned long CopyTime;
    unsigned long CompileTime;
    unsigned long TotalTime;
};

inline Filter::Filter(const String &name) : accumulate(false), accumulated(0), Name(name), RenderTime(0), CopyTime(0), CompileTime(0), TotalTime(0)
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