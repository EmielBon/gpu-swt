//
//  SWTHelperGPU.cpp
//  OGLTest
//
//  Created by Emiel Bon on 25-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTHelperGPU.h"

#include "FrameBuffer.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "ContentLoader.h"
#include "Program.h"
#include "Texture.h"
#include "DrawableRect.h"
#include "RenderWindow.h"
#include "BoundingBox.h"
#include "ImgProc.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GLError.h"
#include "Profiling.h"
#include "GrayFilter.h"
#include "SobelFilter.h"
#include "GaussianFilter.h"
#include "CannyFilter.h"
#include "SWTFilter.h"
#include "ConnectedComponentsFilter.h"
#include "TextureUtil.h"

#define USE_NEW_SYSTEM

Ptr<Texture> Grayscale(const Texture &texture);
// Old version, retained for performance evaluation later. Small shaders, but 5 passes and (3 + 2) * 2 + 2 = 12 texel fetches per pixel
Ptr<Texture> Sobel(const Texture &texture);
// Optimized version. Slightly larger shaders but only 2 passes and less texel fetches than above, 5 + 4 = 9 texel fetches per pixel
Ptr<Texture> Sobel2(const Texture &texture);
// Optimized version with linear sampling, only 3 * 3 = 9 texel fetches per pixel for a 5x5 convolution kernel, instead of 25
Ptr<Texture> GaussianBlur(const Texture &texture);
// Outputs gradient direction and magnitude instead of vector
Ptr<Texture> CannySobel(const Texture &texture);
Ptr<Texture> Canny(const Texture &texture);
Ptr<Texture> StrokeWidthTransform(const Texture &edges, const Texture &gradients, GradientDirection direction);
Ptr<Texture> ConnectedComponents(const Texture &strokeWidths);
Ptr<Program> LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource);
Ptr<Program> LoadScreenSpaceProgram(const String &name);
void StartAccumulatedRender();
Ptr<Texture> Render(const String &name = "");
void EndAccumulatedRender();
Ptr<Texture> ApplyPass(Ptr<Filter> filter, Ptr<Texture> input = nullptr);
void AddTexture(Ptr<Texture> texture);

TimeSpan renderTime;
TimeSpan copyTime;
TimeSpan compileTime;
TimeSpan accumulated;
bool accumulate = false;
Ptr<Texture> accumulatedTexture = nullptr;

List<BoundingBox> SWTHelperGPU::StrokeWidthTransform(const cv::Mat &input)
{
    auto t = now();
    
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    //glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    //glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);
    
    glDisable(GL_DITHER);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glPixelZoom(1.0,1.0);
    
    int width  = input.size().width;
    int height = input.size().height;
    
#ifdef USE_NEW_SYSTEM
    auto grayFilter  = New<GrayFilter>();
    auto swtFilter   = New<SWTFilter>();
    auto connectedComponentsFilter = New<ConnectedComponentsFilter>();
    
    grayFilter->DoLoadShaderPrograms();
    swtFilter->DoLoadShaderPrograms();
    connectedComponentsFilter->DoLoadShaderPrograms();
#endif
    
    // Create a Texture from the input
    Ptr<Texture> texture = textureFromImage<cv::Vec3f>(input);
    
    // Create the framebuffer attachments
    Ptr<Texture>      colorf       = New<Texture     >(width, height, GL_RGBA, GL_FLOAT, GL_NEAREST);
    Ptr<RenderBuffer> depthStencil = New<RenderBuffer>(width, height, RenderBuffer::Type::DepthStencil);
    
    // Create and setup framebuffer
    FrameBuffer frameBuffer;
    frameBuffer.Attach(colorf);
    
    // Create a full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    GraphicsDevice::SetDefaultBuffers(rect.VertexBuffer, rect.IndexBuffer);
    GraphicsDevice::UseDefaultBuffers();
    
    frameBuffer.Bind();

#ifdef USE_NEW_SYSTEM
    auto gray        = ApplyPass(grayFilter, texture);
    grayFilter.reset();
    swtFilter->Input = gray;
    swtFilter->GradientDirection = GradientDirection::With;
    auto swt1        = ApplyPass(swtFilter, gray);
    swtFilter->GradientDirection = GradientDirection::Against;
    auto swt2        = ApplyPass(swtFilter, gray);
    gray.reset();
    swtFilter.reset();
    auto components1 = ApplyPass(connectedComponentsFilter, swt2);
    swt1.reset();
    components1.reset();
#else
    auto gray      = Grayscale(*texture);
    auto gradients = Sobel2(*gray);
    auto blurred   = GaussianBlur(*gray);
    auto edges     = Canny(*blurred);
    auto swt1      = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::With);
    auto swt2      = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::Against);
    //auto connectedComponents = ConnectedComponents(*swt1);
    //RenderWindow::Instance().AddTexture(ImgProc::CalculateEdgeMap(ImgProc::ConvertToGrayscale(input)), "Edges (OpenCV Canny)");*/
#endif
    
    auto totalTime = now() - t;
    
    auto misc = totalTime - renderTime - copyTime - compileTime;
    
    printf("%s: T(%.1fms) R(%.1fms=%.1f%%) Cpy(%.1fms=%.1f%%) Cpl(%.1fms=%.1f%%) M(%.1fms=%.1f%%)\n", "Total",
           GetTimeMsec(totalTime),
           GetTimeMsec(renderTime),
           renderTime * 100.0f / totalTime,
           GetTimeMsec(copyTime),
           copyTime * 100.0f / totalTime,
           GetTimeMsec(compileTime),
           compileTime * 100.0f / totalTime,
           GetTimeMsec(misc),
           misc * 100.0f / totalTime
    );

    printf("Textures: Active %i Peak %i\n", Texture::ActiveTextureCount, Texture::PeakTextureCount);
    
    return List<BoundingBox>();
}

void StartAccumulatedRender()
{
    accumulate = true;
    accumulated = TimeSpan(0);
}

#ifdef PROFILING

Ptr<Texture> Render(const String &name)
{
    auto frameBuffer = FrameBuffer::GetCurrentlyBound();
    glFinish();
    auto t = now();
    GraphicsDevice::DrawPrimitives();
    glFinish();
    auto f = now() - t;
    accumulated += f;
    if (!accumulate)
    {
        renderTime += f;
        PrintTime(name, f);
    }
    t = now();
    auto dest = frameBuffer->ColorAttachment0->GetEmptyClone();
    frameBuffer->CopyColorAttachment(*dest);
    glFinish();
    copyTime += now() - t;
    if (name != "")
        RenderWindow::Instance().AddTexture(dest, name);
    accumulatedTexture = dest;
    return dest;
}

#else

Ptr<Texture> Render(const String &name)
{
    auto& frameBuffer = FrameBuffer::GetCurrentlyBound();
    GraphicsDevice::DrawPrimitives();
    auto result = frameBuffer.CopyColorAttachment();
    if (name != "")
        RenderWindow::Instance().AddTexture(result, name);
    accumulatedTexture = result;
    return result;
}

#endif

void EndAccumulatedRender(const String& name)
{
    accumulate = false;
    renderTime += accumulated;
    PrintTime(name, accumulated);
    RenderWindow::Instance().AddTexture(accumulatedTexture, name);
}

Ptr<Texture> ApplyPass(Ptr<Filter> filter, Ptr<Texture> input)
{
    auto output = FrameBuffer::GetCurrentlyBound()->ColorAttachment0->GetEmptyClone();
    
    if (input)
        filter->Input = input;
    filter->Apply(output);
    renderTime  += filter->RenderTime;
    copyTime    += filter->CopyTime;
    compileTime += filter->CompileTime;
    
    RenderWindow::Instance().AddFrameBufferSnapshot(filter->Name);
    return output;
}

Ptr<Texture> Grayscale(const Texture &texture)
{
    auto grayscale = LoadScreenSpaceProgram("Grayscale");
    
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(texture);
    auto result = Render("Grayscale");
    
    return result;
}

Ptr<Texture> Sobel(const Texture &texture)
{
    // Load the shaders
    /*auto sobelHor1 = LoadScreenSpaceProgram("SobelHor1");
    auto sobelHor2 = LoadScreenSpaceProgram("SobelHor2");
    auto sobelVer1 = LoadScreenSpaceProgram("SobelVer1");
    auto sobelVer2 = LoadScreenSpaceProgram("SobelVer2");
    auto gradientsFromSobel = LoadScreenSpaceProgram("GradientsFromSobel");
    
    // Create references to the render target textures
    Ptr<Texture> gradientH, gradientV, gradients;
    
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobelHor1->Use();
    sobelHor1->Uniforms["Texture"].SetValue(texture);
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobelHor2->Use();
    sobelHor2->Uniforms["Texture"].SetValue(*gradientH);
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer1->Use();
    sobelVer1->Uniforms["Texture"].SetValue(texture);
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer2->Use();
    sobelVer2->Uniforms["Texture"].SetValue(*gradientV);
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    gradientsFromSobel->Use();
    gradientsFromSobel->Uniforms["SobelHor"].SetValue(*gradientH);
    gradientsFromSobel->Uniforms["SobelVer"].SetValue(*gradientV);
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    
    return gradients;*/
    return nullptr;
}

Ptr<Texture> Sobel2(const Texture &texture)
{
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("Sobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    scharrAveraging = Render("Sobel1");
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    gradients = Render("Sobel2");
    
    RenderWindow::Instance().AddTexture(gradients, "Gradients (Sobel/Scharr)");
    
    return gradients;
}

Ptr<Texture> CannySobel(const Texture &texture)
{
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("CannySobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    scharrAveraging = Render("Sobel 1 (Canny)");
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    gradients = Render("CannySobel 2");
    
    return gradients;
}

Ptr<Texture> GaussianBlur(const Texture &texture)
{
    auto gaussianH = LoadScreenSpaceProgram("GaussianBlurH");
    auto gaussianV = LoadScreenSpaceProgram("GaussianBlurV");
    
    Ptr<Texture> gaussian1, gaussian2;
    
    gaussianH->Use();
    gaussianH->Uniforms["Texture"].SetValue(texture);
    gaussian1 = Render("Gaussian Blur (hor)");
    
    gaussianV->Use();
    gaussianV->Uniforms["Texture"].SetValue(*gaussian1);
    gaussian2 = Render("Gaussian Blur (ver)");
    
    RenderWindow::Instance().AddTexture(gaussian2, "Blurred (Gaussian)");
    
    return gaussian2;
}

Ptr<Texture> Canny(const Texture &texture)
{
    auto canny = LoadScreenSpaceProgram("Canny");
    auto gradients = CannySobel(texture);
    
    Ptr<Texture> edges;
    
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
    
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(*gradients);
    edges = Render("Edges (Canny)");
    
    glDisable(GL_STENCIL_TEST);
    
    return edges;
}

// todo: pack gradient direction, stroke width and average stroke width in one texture
Ptr<Texture> StrokeWidthTransform(const Texture &edges, const Texture &gradients, GradientDirection direction)
{
    bool darkOnLight = direction == GradientDirection::With;
    
    auto strokeWidthTransform1 = LoadScreenSpaceProgram("StrokeWidthTransform1");
    auto strokeWidthTransform2 = ContentLoader::Load<Program>("StrokeWidthTransform2");
    auto strokeWidthTransform3 = LoadScreenSpaceProgram("StrokeWidthTransform3");
    auto scaleColor = LoadScreenSpaceProgram("ScaleColor");
    
    auto quadVertexBuffer = GraphicsDevice::VertexBuffer;
    auto quadIndexBuffer = GraphicsDevice::IndexBuffer;
    
    auto linesVertexBuffer = New<::VertexBuffer>();
    auto linesIndexBuffer = New<::IndexBuffer>(PrimitiveType::Lines);
    
    Ptr<Texture> strokeWidthValues, strokeWidthTransform, avgStrokeWidthValues, avgStrokeWidthTransform;
    
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    strokeWidthTransform1->Use();
    strokeWidthTransform1->Uniforms["Edges"].SetValue(edges);
    strokeWidthTransform1->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform1->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    strokeWidthValues = Render("Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    List<VertexPositionTexture> vertices;
    GLfloat buffer[edges.GetWidth() * edges.GetHeight()];
    strokeWidthValues->GetTextureImage(GL_RED, GL_FLOAT, buffer);
    
    for(int i = 0; i < edges.GetWidth(); ++i)
    for(int j = 0; j < edges.GetHeight(); ++j)
    {
        if (buffer[i + j * edges.GetWidth()] != 0.0f)
        {
            VertexPositionTexture v1, v2;
            v1.Position = Vector3(i, j, 0); // z == 0 = Use directly
            v2.Position = Vector3(i, j, 1); // z == 1 = Scatter position to end point
            vertices.push_back(v1);
            vertices.push_back(v2);
        }
    }
    
    // todo: skip index buffer for line drawing
    List<GLuint> indices( vertices.size() );
    GLuint counter = 0;
    for(auto& index : indices)
        index = counter++;
    
    linesVertexBuffer->SetData(vertices);
    linesIndexBuffer->SetData(indices);
    
    GraphicsDevice::SetBuffers(linesVertexBuffer, linesIndexBuffer);
    
    strokeWidthTransform2->Use();
    // todo: need not pass x,y gradients, only direction matters
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    strokeWidthTransform = Render((String("Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str());
    
    GraphicsDevice::SetBuffers(quadVertexBuffer, quadIndexBuffer);
    
    glDisable(GL_DEPTH_TEST);
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 2, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    strokeWidthTransform3->Use();
    strokeWidthTransform3->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform3->Uniforms["LineLengths"].SetValue(*strokeWidthTransform);
    strokeWidthTransform3->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    avgStrokeWidthValues = Render("Average Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GraphicsDevice::SetBuffers(linesVertexBuffer, linesIndexBuffer);
    
    strokeWidthTransform2->Use();
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*avgStrokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    avgStrokeWidthTransform = Render((String("Average Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str());
    
    glDisable(GL_DEPTH_TEST);
    
    GraphicsDevice::SetBuffers(quadVertexBuffer, quadIndexBuffer);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    scaleColor->Use();
    scaleColor->Uniforms["Texture"].SetValue(*avgStrokeWidthTransform);
    scaleColor->Uniforms["Scale"].SetValue(1.0f / 50);
    auto scaledStrokeWidths = Render("Average Stroke Widths (scaled)");
    
    return avgStrokeWidthTransform;
}

Ptr<Texture> ConnectedComponents(const Texture &strokeWidths)
{
    int width = strokeWidths.GetWidth();
    int height = strokeWidths.GetHeight();
    
    auto encode         = LoadScreenSpaceProgram("Encode");
    auto verticalRun    = LoadScreenSpaceProgram("VerticalRuns");
    auto normal         = LoadScreenSpaceProgram("Normal");
    auto color          = LoadScreenSpaceProgram("Color");
    auto gatherNeighbor = LoadProgram("GatherNeighbor", "GatherScatter");
    auto updateColumn   = LoadScreenSpaceProgram("UpdateColumn");
    auto scatterBack    = LoadProgram("ScatterBack", "GatherScatter");
    
    Ptr<Texture> encodedPositions, verticalRuns;
    
    encode->Use();
    encode->Uniforms["Texture"].SetValue(strokeWidths);
    encode->Uniforms["BackgroundColor"].SetValue(0.0f);
    encodedPositions = Render("Connected Components 1 Encode Positions");
    
    verticalRun->Use();
    verticalRuns = encodedPositions;
    
    StartAccumulatedRender();
    int log_h = (int)(log2f(strokeWidths.GetHeight()));
    for(int i = 0; i <= log_h; ++i)
    {
        verticalRun->Uniforms["Texture"].SetValue(*verticalRuns);
        verticalRun->Uniforms["PassIndex"].SetValue(i);
        verticalRuns = Render();
    }
    EndAccumulatedRender("Connected Components 2 Vertical Runs");
    
    GLfloat buffer[width * height];
    verticalRuns->GetTextureImage(GL_BLUE, GL_FLOAT, buffer);
    
    auto quadVertices = GraphicsDevice::VertexBuffer;
    auto quadIndices  = GraphicsDevice::IndexBuffer;
    
    Ptr<Texture> tex0, tex1;
    
    tex0 = verticalRuns;
    
    /*for(int column = strokeWidths.GetWidth() - 2; column >= 0; --column)
    {
        List<VertexPositionTexture> vertices;
        for (int y = 0; y < strokeWidths.GetHeight(); ++y)
        {
            float rootID = buffer[column + y * width];
            if (rootID != 0.0)
                vertices.push_back(VertexPositionTexture(Vector3(column, y, rootID), Vector2(0, 0)));
        }
        
        List<GLuint> indices( vertices.size() );
        GLuint counter = 0;
        for(auto& index : indices)
            index = counter++;
        
        Ptr<VertexBuffer> pixelVertices = New<VertexBuffer>();
        pixelVertices->SetData(vertices);
        Ptr<IndexBuffer> pixelIndices = New<IndexBuffer>();
        pixelIndices->SetData(indices);
        
        device.VertexBuffer = pixelVertices;
        device.IndexBuffer  = pixelIndices;
        
        glDepthFunc(GL_GREATER);
        glClearDepth(0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        gatherNeighbor->Use();
        gatherNeighbor->Uniforms["Texture"].SetValue(*verticalRuns);
        columnProcessing1 = Render(frameBuffer, PrimitiveType::Points);
        
        glDisable(GL_DEPTH_TEST);
        
        device.VertexBuffer = quadVertices;
        device.IndexBuffer  = quadIndices;
        
        updateColumn->Use();
        updateColumn->Uniforms["Texture"].SetValue(*columnProcessing1);
        columnProcessing2 = Render(frameBuffer, PrimitiveType::Triangles);
        
        normal->Use();
        normal->Uniforms["Texture"].SetValue(*columnProcessing2);
        Render(frameBuffer, PrimitiveType::Triangles, ColorAttachmentOption::Keep);
        
        glDepthFunc(GL_GREATER);
        glClearDepth(0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        device.VertexBuffer = pixelVertices;
        device.IndexBuffer  = pixelIndices;
        
        scatterBack->Use();
        scatterBack->Uniforms["Texture"].SetValue(*columnProcessing2);
        columnProcessing3 = Render(frameBuffer, PrimitiveType::Points);
        
        glDisable(GL_DEPTH_TEST);
    }*/
    
    return encodedPositions;
}

Ptr<Program> LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource)
{
    List< Ptr<Shader> > shaders;
    
    auto vs = ContentLoader::Load<VertexShader>(vertexShaderSource);
    auto fs = ContentLoader::Load<FragmentShader>(fragmentShaderSource);
    
    shaders.push_back(std::dynamic_pointer_cast<Shader>(vs));
    shaders.push_back(std::dynamic_pointer_cast<Shader>(fs));
    
    return New<Program>(shaders);
}

Ptr<Program> LoadScreenSpaceProgram(const String &name)
{
    return LoadProgram("Trivial", name);
}