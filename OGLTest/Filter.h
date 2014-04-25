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
    
    static Ptr<Program> LoadScreenSpaceProgram(const String &name);
    
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
    unsigned long TotalTime;
};

inline Filter::Filter(const String &name) : accumulate(false), accumulated(0), Name(name), RenderTime(0), CopyTime(0), TotalTime(0)
{
    
}