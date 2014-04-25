//
//  Filter.cpp
//  OGLTest
//
//  Created by Emiel Bon on 22-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Filter.h"
#include "Program.h"
#include "Profiling.h"
#include "FrameBuffer.h"
#include "GraphicsDevice.h"

Ptr<Texture> Filter::Apply(const Texture &input)
{
    auto t = now();
    
    LoadShaderPrograms();
    CompileTime += now() - t;
    
    auto output = PerformSteps(input);
    
    TotalTime = now() - t;
#ifdef PROFILING
    PrintProfilingInfo();
#endif
    return output;
}

Ptr<Texture> Filter::Render(const String &name)
{
    auto f = now();
    GraphicsDevice::DrawPrimitives();
    glFinish();
    f = now() - f;
    accumulated += f;
    if (!accumulate)
    {
        accumulated = 0;
        RenderTime += f;
    }
    f = now();
    auto result = FrameBuffer::GetCurrentlyBound().CopyColorAttachment();
    glFinish();
    CopyTime += now() - f;
    return result;
}

Ptr<Program> Filter::LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource)
{
    return Program::LoadFromSources(vertexShaderSource, fragmentShaderSource);
}

Ptr<Texture> Filter::ApplyFilter(Filter &filter, const Texture &input)
{
    auto result  = filter.Apply(input);
    RenderTime  += filter.RenderTime;
    CopyTime    += filter.CopyTime;
    CompileTime += filter.CompileTime;
    return result;
}

void Filter::PrintProfilingInfo() const
{
    unsigned long misc = TotalTime - RenderTime - CopyTime - CompileTime;
    
    printf("%s: T(%.1fms) R(%.1fms=%.1f%%) Cpy(%.1fms=%.1f%%) Cpl(%.1fms=%.1f%%) M(%.1fms=%.1f%%)\n",
           Name.c_str(),
           GetTimeMsec(TotalTime),
           GetTimeMsec(RenderTime),
           RenderTime * 100.0f / TotalTime,
           GetTimeMsec(CopyTime),
           CopyTime * 100.0f / TotalTime,
           GetTimeMsec(CompileTime),
           CompileTime * 100.0f / TotalTime,
           GetTimeMsec(misc),
           misc * 100.0f / TotalTime);
}