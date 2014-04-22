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

enum class ColorAttachmentOption
{
    CreateNew,
    Keep,
};

Ptr<Texture> Grayscale(const Texture &texture, FrameBuffer &frameBuffer);
// Old version, retained for performance evaluation later. Small shaders, but 5 passes and (3 + 2) * 2 + 2 = 12 texel fetches per pixel
Ptr<Texture> Sobel(const Texture &texture, FrameBuffer &frameBuffer);
// Optimized version. Slightly larger shaders but only 2 passes and less texel fetches than above, 5 + 4 = 9 texel fetches per pixel
Ptr<Texture> Sobel2(const Texture &texture, FrameBuffer &frameBuffer);
// Optimized version with linear sampling, only 3 * 3 = 9 texel fetches per pixel for a 5x5 convolution kernel, instead of 25
Ptr<Texture> GaussianBlur(const Texture &texture, FrameBuffer &frameBuffer);
// Outputs gradient direction and magnitude instead of vector
Ptr<Texture> CannySobel(const Texture &texture, FrameBuffer &frameBuffer);
Ptr<Texture> Canny(const Texture &texture, FrameBuffer &frameBuffer);
Ptr<Texture> StrokeWidthTransform(const Texture &edges, const Texture &gradients, GradientDirection direction, FrameBuffer &frameBuffer);
Ptr<Texture> ConnectedComponents(const Texture &strokeWidths, FrameBuffer &frameBuffer);
Ptr<Program> LoadProgram(const String &vertexShaderSource, const String &fragmentShaderSource);
Ptr<Program> LoadScreenSpaceProgram(const String &name);
Ptr<Texture> Render(FrameBuffer &frameBuffer, PrimitiveType primitiveType = PrimitiveType::Triangles, ColorAttachmentOption option = ColorAttachmentOption::CreateNew);
Ptr<Texture> RenderProfiled(FrameBuffer &frameBuffer, const char* name, PrimitiveType primitiveType = PrimitiveType::Triangles, ColorAttachmentOption option = ColorAttachmentOption::CreateNew);

unsigned long totalClicks = 0;

List<BoundingBox> SWTHelperGPU::StrokeWidthTransform(const cv::Mat &input)
{
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);
    
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = input.size().width;
    int height = input.size().height;
    
    // Load the framebuffers
    FrameBuffer frameBuffer1(width, height, GL_RGB, GL_UNSIGNED_BYTE);
    FrameBuffer frameBuffer2(width, height, GL_RGB, GL_FLOAT, RenderBuffer::Type::DepthStencil);
    
    // Load the full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    device.VertexBuffer = rect.VertexBuffer;
    device.IndexBuffer  = rect.IndexBuffer;
    
    // Create a Texture from the input
    Texture texture(input);
    
    auto gray = Grayscale(input, frameBuffer1);
    auto gradients = Sobel2(*gray, frameBuffer2);
    auto blurred = GaussianBlur(*gray, frameBuffer2);
    auto edges = Canny(*blurred, frameBuffer2);
    auto strokeWidths1 = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::With, frameBuffer2);
    auto strokeWidths2 = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::Against, frameBuffer2);
    auto connectedComponents = ConnectedComponents(*strokeWidths1, frameBuffer2);
    //RenderWindow::Instance().AddTexture(ImgProc::CalculateEdgeMap(ImgProc::ConvertToGrayscale(input)), "Edges (OpenCV Canny)");*/
    
    printf("Total time: %.1fms\n", totalClicks * 1000.0f / CLOCKS_PER_SEC);
    
    return List<BoundingBox>();
}

Ptr<Texture> Render(FrameBuffer &frameBuffer, PrimitiveType primitiveType, ColorAttachmentOption option /* = ColorAttachmentOption::CreateNew */)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    device.DrawPrimitives(primitiveType);
    auto result = frameBuffer.Texture;
    if (option == ColorAttachmentOption::CreateNew)
        frameBuffer.CreateNewColorAttachment0();

    return result;
}

Ptr<Texture> RenderProfiled(FrameBuffer &frameBuffer, const char *name, PrimitiveType primitiveType, ColorAttachmentOption option /* = ColorAttachmentOption::CreateNew */)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    clock_t f;
    f = clock();
    device.DrawPrimitives(primitiveType);
    glFinish();
    f = clock() - f;
    totalClicks += f;
    printf("%s (%.1fms)\n", name, (f * 1000.0f) / CLOCKS_PER_SEC);
    auto result = frameBuffer.Texture;
    if (option == ColorAttachmentOption::CreateNew)
        frameBuffer.CreateNewColorAttachment0();
    
    RenderWindow::Instance().AddTexture(result, name);
    
    return result;
}

Ptr<Texture> Grayscale(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto grayscale = LoadScreenSpaceProgram("Grayscale");
    
    frameBuffer.Bind();
    
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(texture);
    auto result = RenderProfiled(frameBuffer, "Grayscale");
    
    return result;
}

Ptr<Texture> Sobel(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    // Load the shaders
    auto sobelHor1 = LoadScreenSpaceProgram("SobelHor1");
    auto sobelHor2 = LoadScreenSpaceProgram("SobelHor2");
    auto sobelVer1 = LoadScreenSpaceProgram("SobelVer1");
    auto sobelVer2 = LoadScreenSpaceProgram("SobelVer2");
    auto gradientsFromSobel = LoadScreenSpaceProgram("GradientsFromSobel");
    
    // Create references to the render target textures
    Ptr<Texture> gradientH, gradientV, gradients;
    
    frameBuffer.Bind();
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobelHor1->Use();
    sobelHor1->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobelHor2->Use();
    sobelHor2->Uniforms["Texture"].SetValue(*gradientH);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer1->Use();
    sobelVer1->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer2->Use();
    sobelVer2->Uniforms["Texture"].SetValue(*gradientV);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    gradientsFromSobel->Use();
    gradientsFromSobel->Uniforms["SobelHor"].SetValue(*gradientH);
    gradientsFromSobel->Uniforms["SobelVer"].SetValue(*gradientV);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gradients;
}

Ptr<Texture> Sobel2(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("Sobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    frameBuffer.Bind();
    
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    scharrAveraging = RenderProfiled(frameBuffer, "Sobel1");
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    gradients = RenderProfiled(frameBuffer, "Sobel2");
    
    RenderWindow::Instance().AddTexture(gradients, "Gradients (Sobel/Scharr)");
    
    return gradients;
}

Ptr<Texture> CannySobel(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("CannySobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    frameBuffer.Bind();
    
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    scharrAveraging = RenderProfiled(frameBuffer, "CannySobel 1");
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    gradients = RenderProfiled(frameBuffer, "CannySobel 2");
    
    return gradients;
}

Ptr<Texture> GaussianBlur(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto gaussianH = LoadScreenSpaceProgram("GaussianBlurH");
    auto gaussianV = LoadScreenSpaceProgram("GaussianBlurV");
    
    Ptr<Texture> gaussian1, gaussian2;
    
    frameBuffer.Bind();
    
    gaussianH->Use();
    gaussianH->Uniforms["Texture"].SetValue(texture);
    gaussian1 = RenderProfiled(frameBuffer, "Gaussian Blur (hor)");
    
    gaussianV->Use();
    gaussianV->Uniforms["Texture"].SetValue(*gaussian1);
    gaussian2 = RenderProfiled(frameBuffer, "Gaussian Blur (ver)");
    
    RenderWindow::Instance().AddTexture(gaussian2, "Blurred (Gaussian)");
    
    return gaussian2;
}

Ptr<Texture> Canny(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto canny = LoadScreenSpaceProgram("Canny");
    auto gradients = CannySobel(texture, frameBuffer);
    
    Ptr<Texture> edges;
    
    frameBuffer.Bind();
    
    glClearStencil(0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
    
    canny->Use();
    canny->Uniforms["Gradients"].SetValue(*gradients);
    edges = RenderProfiled(frameBuffer, "Edges (Canny)");
    
    glDisable(GL_STENCIL_TEST);
    
    return edges;
}

// todo: pack gradient direction, stroke width and average stroke width in one texture
Ptr<Texture> StrokeWidthTransform(const Texture &edges, const Texture &gradients, GradientDirection direction, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    bool darkOnLight = direction == GradientDirection::With;
    
    auto strokeWidthTransform1 = LoadScreenSpaceProgram("StrokeWidthTransform1");
    auto strokeWidthTransform2 = ContentLoader::Load<Program>("StrokeWidthTransform2");
    auto strokeWidthTransform3 = LoadScreenSpaceProgram("StrokeWidthTransform3");
    auto scaleColor = LoadScreenSpaceProgram("ScaleColor");
    
    auto quadVertexBuffer = device.VertexBuffer;
    auto quadIndexBuffer = device.IndexBuffer;
    
    auto linesVertexBuffer = New<::VertexBuffer>();
    auto linesIndexBuffer = New<::IndexBuffer>();
    
    Ptr<Texture> strokeWidthValues, strokeWidthTransform, avgStrokeWidthValues, avgStrokeWidthTransform;
    
    frameBuffer.Bind();
    
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
    strokeWidthValues = RenderProfiled(frameBuffer, "Stroke Width values");
    
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
    
    device.VertexBuffer = linesVertexBuffer;
    device.IndexBuffer = linesIndexBuffer;
    
    strokeWidthTransform2->Use();
    // todo: need not pass x,y gradients, only direction matters
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(darkOnLight);
    strokeWidthTransform = RenderProfiled(frameBuffer, (String("Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str(), PrimitiveType::Lines);
    
    device.VertexBuffer = quadVertexBuffer;
    device.IndexBuffer = quadIndexBuffer;
    
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
    avgStrokeWidthValues = RenderProfiled(frameBuffer, "Average Stroke Width values");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    device.VertexBuffer = linesVertexBuffer;
    device.IndexBuffer = linesIndexBuffer;
    
    strokeWidthTransform2->Use();
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["LineLengths"].SetValue(*strokeWidthValues);
    strokeWidthTransform2->Uniforms["Values"].SetValue(*avgStrokeWidthValues);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    avgStrokeWidthTransform = RenderProfiled(frameBuffer, (String("Average Stroke Width Transform (") + (darkOnLight ? "with" : "against") + " the gradient)").c_str(), PrimitiveType::Lines);
    
    glDisable(GL_DEPTH_TEST);
    
    device.VertexBuffer = quadVertexBuffer;
    device.IndexBuffer = quadIndexBuffer;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    scaleColor->Use();
    scaleColor->Uniforms["Texture"].SetValue(*avgStrokeWidthTransform);
    scaleColor->Uniforms["Scale"].SetValue(1.0f / 50);
    auto scaledStrokeWidths = Render(frameBuffer);
    
    RenderWindow::Instance().AddTexture(scaledStrokeWidths, "Average Stroke Widths (scaled)");
    
    frameBuffer.Unbind();
    
    return avgStrokeWidthTransform;
}

Ptr<Texture> ConnectedComponents(const Texture &strokeWidths, FrameBuffer &frameBuffer)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width = strokeWidths.GetWidth();
    int height = strokeWidths.GetHeight();
    
    auto encode = LoadScreenSpaceProgram("Encode");
    auto verticalRun = LoadScreenSpaceProgram("VerticalRuns");
    auto normal = LoadScreenSpaceProgram("Normal");
    auto color = LoadScreenSpaceProgram("Color");
    auto gatherNeighbor = LoadProgram("GatherNeighbor", "GatherScatter");
    auto updateColumn = LoadScreenSpaceProgram("UpdateColumn");
    auto scatterBack = LoadProgram("ScatterBack", "GatherScatter");
    
    Ptr<Texture> encodedPositions, verticalRuns;
    
    frameBuffer.Bind();
    
    encode->Use();
    encode->Uniforms["Texture"].SetValue(strokeWidths);
    encode->Uniforms["BackgroundColor"].SetValue(0.0f);
    encodedPositions = RenderProfiled(frameBuffer, "Connected Components 1 Encode Positions");
    
    verticalRun->Use();
    verticalRuns = encodedPositions;
    
    int log_h = (int)(log2f(strokeWidths.GetHeight()));
    for(int i = 0; i <= log_h; ++i)
    {
        verticalRun->Uniforms["Texture"].SetValue(*verticalRuns);
        verticalRun->Uniforms["PassIndex"].SetValue(i);
        verticalRuns = Render(frameBuffer);
    }
    
    RenderWindow::Instance().AddTexture(verticalRuns, "Connected Components 2 Vertical Runs");
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    normal->Use();
    normal->Uniforms["Texture"].SetValue(*verticalRuns);
    Render(frameBuffer, PrimitiveType::Triangles, ColorAttachmentOption::Keep);
    
    GLfloat buffer[width * height];
    verticalRuns->GetTextureImage(GL_BLUE, GL_FLOAT, buffer);
    
    auto quadVertices = device.VertexBuffer;
    auto quadIndices  = device.IndexBuffer;
    
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
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    List< Ptr<Shader> > shaders;
    
    auto vs = ContentLoader::Load<VertexShader>(vertexShaderSource);
    auto fs = ContentLoader::Load<FragmentShader>(fragmentShaderSource);
    
    shaders.push_back(std::dynamic_pointer_cast<Shader>(vs));
    shaders.push_back(std::dynamic_pointer_cast<Shader>(fs));
    
    return New<Program>(&device, shaders);
}

Ptr<Program> LoadScreenSpaceProgram(const String &name)
{
    return LoadProgram("Trivial", name);
}