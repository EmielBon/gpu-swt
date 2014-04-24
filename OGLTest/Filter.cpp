//
//  Filter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Filter.h"
#include "Program.h"

void Filter::Apply()
{
    LoadShaderPrograms();
}

Ptr<Program> Filter::LoadScreenSpaceProgram(const String &name)
{
    return Program::LoadFromSources("Trivial", name);
}

void Filter::AddScreenSpaceProgram(const String &name)
{
    Programs[name] = LoadScreenSpaceProgram(name);
}

void Filter::AddProgram(const String &name)
{
    Programs[name] = Program::Load(name);
}