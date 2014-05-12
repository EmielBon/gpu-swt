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
#include "Texture.h"

void Filter::ReserveColorBuffers(int count)
{
    ColorBuffers.clear();
    for(int i = 0; i < count; ++i)
        ColorBuffers.push_back( Input->GetEmptyClone() );
}

void Filter::Apply(Ptr<Texture> output)
{
    TotalTime = RenderTime = CopyTime = CompileTime = 0;
    
    if (!Input)
        throw std::runtime_error(String("No input specified for filter: ") + Name);
        
    auto t = now();
    
    if (!initialized)
    {
        LoadShaderPrograms();
        CompileTime += now() - t;
        Initialize();
        initialized = true;
    }

    PerformSteps(output);
    ColorBuffers.clear();
    
    TotalTime = now() - t;
#ifdef PROFILING
    PrintProfilingInfo();
#endif
}

void Filter::Render(PrimitiveType primitiveType /* = PrimitiveType::Unspecified */, GLenum clearOptions /* = GL_NONE */)
{
    glFinish();
    auto f = now();
    if (clearOptions != GL_NONE);
        glClear(clearOptions);
    if (primitiveType == PrimitiveType::Unspecified)
        GraphicsDevice::DrawPrimitives();
    else
        GraphicsDevice::DrawArrays(primitiveType);
    glFinish();
    RenderTime += now() - f;
}

void Filter::RenderToTexture(Ptr<Texture> destination, PrimitiveType primitiveType /* = PrimitiveType::Unspecified */, GLenum clearOptions /* = GL_NONE */)
{
    SetColorAttachment(destination);
    Render(primitiveType, clearOptions);
}

void Filter::CopyFrameBufferColors(const Texture &dest)
{
    glFinish();
    auto f = now();
    FrameBuffer::GetCurrentlyBound()->CopyColorAttachment(dest);
    glFinish();
    CopyTime += now() - f;
}

Ptr<Texture> Filter::GetColorAttachment()
{
    return FrameBuffer::GetCurrentlyBound()->ColorAttachment0;
}

void Filter::SetColorAttachment(Ptr<Texture> colorAttachment)
{
    FrameBuffer::GetCurrentlyBound()->Attach(colorAttachment);
}

Ptr<Program> Filter::LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource)
{
    return Program::LoadFromSources(vertexShaderSource, fragmentShaderSource);
}

void Filter::ApplyFilter(Filter &filter, Ptr<Texture> output)
{
    filter.Apply(output);
    RenderTime  += filter.RenderTime;
    CopyTime    += filter.CopyTime;
    CompileTime += filter.CompileTime;
}

void Filter::PrintProfilingInfo() const
{
    auto misc = TotalTime - RenderTime - CopyTime - CompileTime;
    
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