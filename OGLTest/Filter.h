//
//  Filter.h
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class Filter
{
public:
    
    Filter(GraphicsDevice *device);
    
    void Apply();
    
    virtual void LoadShaderPrograms() = 0;
    
protected:
    
    Ptr<Program> LoadScreenSpaceProgram(const String &name);
    
    void AddScreenSpaceProgram(const String &name);
    
    void AddProgram(const String &name);
    
protected:
    
    GraphicsDevice *device;
    Map<String, Ptr<Program>> Programs;
};

inline Filter::Filter(GraphicsDevice *device) : device(device)
{
    
}