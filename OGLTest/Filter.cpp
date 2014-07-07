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
#include "ContentLoader.h"
#include "FrameBuffer.h"
#include "VertexPosition.h"
#include "VertexBuffer.h"

Ptr<VertexBuffer> Filter::PerPixelVertices = nullptr;

void Filter::ReserveColorBuffers(int count)
{
    ColorBuffers.clear();
    for(int i = 0; i < count; ++i)
        ColorBuffers.push_back( GetColorAttachment()->GetEmptyClone() );
}

void Filter::DoLoadShaderPrograms()
{
    if (shadersLoaded)
        return;
#ifdef PROFILING
    glFinish();
    auto t = now();
#endif
    LoadShaderPrograms();
#ifdef PROFILING
    glFinish();
    CompileTime += now() - t;
#endif
    shadersLoaded = true;
}

void Filter::DoInitialize()
{
    if (!initialized)
    {
        Initialize();
        ColorBuffers.clear();
        if (!PerPixelVertices && !Input)
            throw std::runtime_error("Cannot initialize per pixel vertex buffer without input texture");
        if (!PerPixelVertices) // initialize the static per pixel vertexbuffer once
            PreparePerPixelVertices();
    }
    initialized = true;
}

void Filter::Apply(Ptr<Texture> output)
{
    TotalTime = RenderTime = CompileTime = TimeSpan(0);
    
    if (!Input)
        throw std::runtime_error(String("No input specified for filter: ") + Name);
#ifdef PROFILING
    glFinish();
    auto t = now();
#endif
    DoLoadShaderPrograms();
    DoInitialize();
    PerformSteps(output);
    ColorBuffers.clear();
#ifdef PROFILING
    TotalTime = now() - t;
    PrintProfilingInfo();
#endif
}

void Filter::Render(PrimitiveType primitiveType /* = PrimitiveType::Unspecified */, GLenum clearOptions /* = GL_NONE */)
{
#ifdef PROFILING
    glFinish();
    auto f = now();
#endif
    if (clearOptions != GL_NONE);
        glClear(clearOptions);
    if (primitiveType == PrimitiveType::Unspecified)
        GraphicsDevice::DrawPrimitives();
    else
        GraphicsDevice::DrawArrays(primitiveType);
#ifdef PROFILING
    glFinish();
    RenderTime += now() - f;
#endif
}

void Filter::RenderToTexture(Ptr<Texture> destination, PrimitiveType primitiveType /* = PrimitiveType::Unspecified */, GLenum clearOptions /* = GL_NONE */)
{
    SetColorAttachment(destination);
    Render(primitiveType, clearOptions);
}

Ptr<Texture> Filter::GetColorAttachment()
{
    return FrameBuffer::GetCurrentlyBound()->ColorAttachment0;
}

void Filter::SetColorAttachment(Ptr<Texture> colorAttachment)
{
    FrameBuffer::GetCurrentlyBound()->SetColorAttachment(colorAttachment);
}

Ptr<Program> Filter::LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource)
{
    return ContentLoader::Load<Program>(vertexShaderSource, fragmentShaderSource);
}

void Filter::ApplyFilter(Filter &filter, Ptr<Texture> output)
{
    if (!output)
        throw std::runtime_error("Error: output is null");
    filter.Apply(output);
    RenderTime  += filter.RenderTime;
    CompileTime += filter.CompileTime;
}

void Filter::PrintProfilingInfo() const
{
    auto misc = TotalTime - RenderTime - CompileTime;
    
    printf("%s: T(%.1fms) R(%.1fms=%.1f%%) Cpl(%.1fms=%.1f%%) M(%.1fms=%.1f%%)\n",
           Name.c_str(),
           GetTimeMsec(TotalTime),
           GetTimeMsec(RenderTime),
           RenderTime * 100.0f / TotalTime,
           GetTimeMsec(CompileTime),
           CompileTime * 100.0f / TotalTime,
           GetTimeMsec(misc),
           misc * 100.0f / TotalTime);
}

void Filter::PreparePerPixelVertices()
{
    int width  = Input->GetWidth();
    int height = Input->GetHeight();
    
    List<VertexPosition> vertices;
    for (int x = 0; x < width;  ++x)
    for (int y = 0; y < height; ++y)
        vertices.push_back(VertexPosition(Vector3(x, y, 0)));
    
    PerPixelVertices = New<VertexBuffer>();
    PerPixelVertices->SetData(vertices);
}
