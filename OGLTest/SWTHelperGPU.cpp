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
Ptr<Program> LoadScreenSpaceProgram(const String &name);

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
    FrameBuffer frameBuffer2(width, height, GL_RGB, GL_FLOAT);
    FrameBuffer frameBuffer3(width, height, GL_RGB, GL_FLOAT, RenderBuffer::Type::DepthStencil);
    
    // Load the full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    device.VertexBuffer = rect.VertexBuffer;
    device.IndexBuffer  = rect.IndexBuffer;
    
    // Create a Texture from the input
    Texture texture(input);
    
    auto gray = Grayscale(input, frameBuffer1);
    auto gradients = Sobel2(*gray, frameBuffer2);
    auto blurred = GaussianBlur(*gray, frameBuffer2);
    auto edges = Canny(*blurred, frameBuffer3);
    auto strokeWidths1 = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::With, frameBuffer3);
    auto strokeWidths2 = ::StrokeWidthTransform(*edges, *gradients, GradientDirection::Against, frameBuffer3);
    
    RenderWindow::Instance().AddTexture(gray, "Grayscale");
    RenderWindow::Instance().AddTexture(gradients, "Gradients (Sobel/Scharr)");
    RenderWindow::Instance().AddTexture(blurred, "Blurred (Gaussian)");
    RenderWindow::Instance().AddTexture(edges, "Edges (Canny)");
    //RenderWindow::Instance().AddTexture(ImgProc::CalculateEdgeMap(ImgProc::ConvertToGrayscale(input)), "Edges (OpenCV Canny)");
    RenderWindow::Instance().AddTexture(strokeWidths1, "Stroke Width Transform (with the gradient)");
    RenderWindow::Instance().AddTexture(strokeWidths2, "Stroke Width Transform (against the gradient)");
    
    return List<BoundingBox>();
}

Ptr<Texture> Grayscale(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    auto grayscale = LoadScreenSpaceProgram("Grayscale");
    
    frameBuffer.Bind();
    
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::Triangles);
    auto result = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
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
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("Sobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    frameBuffer.Bind();
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::Triangles);
    scharrAveraging = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gradients;
}

Ptr<Texture> CannySobel(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = texture.GetWidth();
    int height = texture.GetHeight();
    Vector2 size(width, height);
    
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("CannySobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    frameBuffer.Bind();
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    sobel1->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::Triangles);
    scharrAveraging = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    sobel2->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gradients;
}

Ptr<Texture> GaussianBlur(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    auto gaussianH = LoadScreenSpaceProgram("GaussianBlurH");
    auto gaussianV = LoadScreenSpaceProgram("GaussianBlurV");
    
    Ptr<Texture> gaussian1, gaussian2;
    
    frameBuffer.Bind();
    
    gaussianH->Use();
    gaussianH->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gaussian1 = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    gaussianV->Use();
    gaussianV->Uniforms["Texture"].SetValue(*gaussian1);
    device.DrawPrimitives(PrimitiveType::Triangles);
    gaussian2 = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gaussian2;
}

Ptr<Texture> Canny(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
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
    device.DrawPrimitives(PrimitiveType::Triangles);
    edges = frameBuffer.Texture;
    
    frameBuffer.CreateNewColorAttachment0();
    
    glDisable(GL_STENCIL_TEST);
    
    frameBuffer.Unbind();
    
    return edges;
}

Ptr<Texture> StrokeWidthTransform(const Texture &edges, const Texture &gradients, GradientDirection direction, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    auto alphaTest = LoadScreenSpaceProgram("AlphaTest");
    auto strokeWidthTransform1 = LoadScreenSpaceProgram("StrokeWidthTransform1");
    auto strokeWidthTransform2 = ContentLoader::Load<Program>("StrokeWidthTransform2");
    auto strokeWidthTransform3 = LoadScreenSpaceProgram("StrokeWidthTransform3");
    
    auto quadVertexBuffer = device.VertexBuffer;
    auto quadIndexBuffer = device.IndexBuffer;
    
    auto linesVertexBuffer = New<::VertexBuffer>();
    auto linesIndexBuffer = New<::IndexBuffer>();
    
    Ptr<Texture> strokeWidths, avgStrokeWidths, result, result2;
    
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
    strokeWidthTransform1->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    device.DrawPrimitives(PrimitiveType::Triangles);
    strokeWidths = frameBuffer.Texture;
    GLfloat buffer[edges.GetWidth() * edges.GetHeight()];
    glReadPixels(0, 0, edges.GetWidth(), edges.GetHeight(), GL_RED, GL_FLOAT, buffer);
    frameBuffer.CreateNewColorAttachment0();
    
    RenderWindow::Instance().AddTexture(strokeWidths, "Stroke Widths");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    List<VertexPositionTexture> vertices;
    
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
    List<GLushort> indices( vertices.size() );
    GLushort counter = 0;
    for(auto& index : indices)
        index = counter++;
    
    linesVertexBuffer->SetData(vertices);
    linesIndexBuffer->SetData(indices);
    
    device.VertexBuffer = linesVertexBuffer;
    device.IndexBuffer = linesIndexBuffer;
    
    strokeWidthTransform2->Use();
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    strokeWidthTransform2->Uniforms["StrokeWidths"].SetValue(*strokeWidths);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    device.DrawPrimitives(PrimitiveType::Lines);
    result = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
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
    strokeWidthTransform3->Uniforms["StrokeWidths"].SetValue(*result);
    strokeWidthTransform3->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    device.DrawPrimitives(PrimitiveType::Triangles);
    avgStrokeWidths = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    RenderWindow::Instance().AddTexture(avgStrokeWidths, "Average Stroke Widths");
    
    glDisable(GL_STENCIL_TEST);
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    device.VertexBuffer = linesVertexBuffer;
    device.IndexBuffer = linesIndexBuffer;
    
    strokeWidthTransform2->Use();
    strokeWidthTransform2->Uniforms["Gradients"].SetValue(gradients);
    // todo: do'h, need to pass strokeWidths for position calculation, and avgStrokeWidths for value
    strokeWidthTransform2->Uniforms["StrokeWidths"].SetValue(*avgStrokeWidths);
    strokeWidthTransform2->Uniforms["DarkOnLight"].SetValue(direction == GradientDirection::With);
    device.DrawPrimitives(PrimitiveType::Lines);
    result2 = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();

    glDisable(GL_DEPTH_TEST);
    
    frameBuffer.Unbind();
    
    device.VertexBuffer = quadVertexBuffer;
    device.IndexBuffer = quadIndexBuffer;
    
    return result2;
}

Ptr<Program> LoadScreenSpaceProgram(const String &name)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    List< Ptr<Shader> > shaders;
    
    auto vs = ContentLoader::Load<VertexShader>("Trivial");
    auto fs = ContentLoader::Load<FragmentShader>(name);
    
    shaders.push_back(std::dynamic_pointer_cast<Shader>(vs));
    shaders.push_back(std::dynamic_pointer_cast<Shader>(fs));
    
    return New<Program>(&device, shaders);
}