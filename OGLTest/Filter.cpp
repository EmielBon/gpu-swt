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
    TotalTime = now();
    
    auto output = PerformSteps(input);
    
    TotalTime = now() - TotalTime;
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

Ptr<Program> Filter::LoadScreenSpaceProgram(const String &name)
{
    return Program::LoadFromSources("Trivial", name);
}

void Filter::PrintProfilingInfo() const
{
    printf("%s: T(%.1fms) R(%.1fms=%.1f%%) C(%.1fms=%.1f%%)\n", Name.c_str(), GetTimeMsec(TotalTime), GetTimeMsec(RenderTime), (RenderTime * 100.0f) / TotalTime, GetTimeMsec(CopyTime), (CopyTime * 100.0f) / TotalTime);
}