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

Ptr<Texture> Grayscale(const Texture &texture, FrameBuffer &frameBuffer);
// Old version, retained for performance evaluation later. Small shaders, but 5 passes and (3 + 2) * 2 + 2 = 12 texel fetches per pixel
Ptr<Texture> Sobel(const Texture &texture, FrameBuffer &frameBuffer);
// Optimized version. Slightly larger shaders but only 2 passes and less texel fetches than above, 5 + 4 = 9 texel fetches per pixel
Ptr<Texture> Sobel2(const Texture &texture, FrameBuffer &frameBuffer);
// Optimized version with linear sampling, only 3 * 3 = 9 texel fetches per pixel for a 5x5 convolution kernel, instead of 25
Ptr<Texture> GaussianBlur(const Texture &texture, FrameBuffer &frameBuffer);
Ptr<Texture> Canny(const Texture &texture, FrameBuffer &frameBuffer);
Ptr<Program> LoadScreenSpaceProgram(const String &name);

List<BoundingBox> SWTHelperGPU::StrokeWidthTransform(const cv::Mat &input)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = input.size().width;
    int height = input.size().height;
    
    // Load the framebuffers
    FrameBuffer frameBuffer1(width, height, GL_RGB, GL_UNSIGNED_BYTE);
    FrameBuffer frameBuffer2(width, height, GL_RGB, GL_FLOAT);
    
    // Load the full-screen rect
    DrawableRect rect(-1, -1, 1, 1, 1, 1);
    device.VertexBuffer = rect.VertexBuffer;
    device.IndexBuffer  = rect.IndexBuffer;
    
    // Create a Texture from the input
    Texture texture(input);
    
    auto gray = Grayscale(input, frameBuffer1);
    auto gradients = Sobel2(*gray, frameBuffer2);
    auto blurred = GaussianBlur(*gray, frameBuffer2);
    auto canny = Canny(*gray, frameBuffer2);
    
    RenderWindow::Instance().AddTexture(gray, "Grayscale");
    RenderWindow::Instance().AddTexture(gradients, "Gradients (Sobel/Scharr)");
    RenderWindow::Instance().AddTexture(blurred, "Blurred (Gaussian)");
    RenderWindow::Instance().AddTexture(canny, "Edges (Canny)");
    
    return List<BoundingBox>();
}

Ptr<Texture> Grayscale(const Texture &texture, FrameBuffer &frameBuffer)
{
    auto &device = RenderWindow::Instance().GraphicsDevice;
    auto grayscale = LoadScreenSpaceProgram("Grayscale");
    
    frameBuffer.Bind();
    
    grayscale->Use();
    grayscale->Uniforms["Texture"].SetValue(texture);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    auto result = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return result;
}

Ptr<Texture> Sobel(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = texture.GetWidth();
    int height = texture.GetHeight();
    Vector2 size(width, height);
    
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
    sobelHor1->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobelHor2->Use();
    sobelHor2->Uniforms["Texture"].SetValue(*gradientH);
    sobelHor2->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradientH = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer1->Use();
    sobelVer1->Uniforms["Texture"].SetValue(texture);
    sobelVer1->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    sobelVer2->Use();
    sobelVer2->Uniforms["Texture"].SetValue(*gradientV);
    sobelVer2->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradientV = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    gradientsFromSobel->Use();
    gradientsFromSobel->Uniforms["SobelHor"].SetValue(*gradientH);
    gradientsFromSobel->Uniforms["SobelVer"].SetValue(*gradientV);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gradients;
}

Ptr<Texture> Sobel2(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = texture.GetWidth();
    int height = texture.GetHeight();
    Vector2 size(width, height);
    
    // Load the shaders
    auto sobel1 = LoadScreenSpaceProgram("Sobel1");
    auto sobel2 = LoadScreenSpaceProgram("Sobel2");
    
    // Create references to the render target textures
    Ptr<Texture> scharrAveraging, gradients;
    
    frameBuffer.Bind();
    // Render gray texture -> FrameBuffer2 with SobelHor1 to gradientH texture
    sobel1->Use();
    sobel1->Uniforms["Texture"].SetValue(texture);
    sobel1->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    scharrAveraging = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    // Render gradientH texture -> FrameBuffer2 with SobelHor2 to gradientH texture
    sobel2->Use();
    sobel2->Uniforms["Texture"].SetValue(*scharrAveraging);
    sobel2->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gradients = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gradients;
}

Ptr<Texture> GaussianBlur(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = texture.GetWidth();
    int height = texture.GetHeight();
    Vector2 size(width, height);
    
    auto gaussianH = LoadScreenSpaceProgram("GaussianBlurH");
    auto gaussianV = LoadScreenSpaceProgram("GaussianBlurV");
    
    Ptr<Texture> gaussian1, gaussian2;
    
    frameBuffer.Bind();
    
    gaussianH->Use();
    gaussianH->Uniforms["Texture"].SetValue(texture);
    gaussianH->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gaussian1 = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    gaussianV->Use();
    gaussianV->Uniforms["Texture"].SetValue(*gaussian1);
    gaussianV->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    gaussian2 = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return gaussian2;
}

Ptr<Texture> Canny(const Texture &texture, FrameBuffer &frameBuffer)
{
    // Get the graphics device
    auto &device = RenderWindow::Instance().GraphicsDevice;
    
    int width  = texture.GetWidth();
    int height = texture.GetHeight();
    Vector2 size(width, height);
    
    auto blurred = GaussianBlur(texture, frameBuffer);
    auto gradients = Sobel2(texture, frameBuffer);
    
    auto roundAngles = LoadScreenSpaceProgram("RoundAngles");
    
    Ptr<Texture> canny;
    
    frameBuffer.Bind();
    
    roundAngles->Use();
    roundAngles->Uniforms["Texture"].SetValue(*gradients);
    roundAngles->Uniforms["TextureSize"].SetValue(size);
    device.DrawPrimitives(PrimitiveType::TriangleList);
    canny = frameBuffer.Texture;
    frameBuffer.CreateNewColorAttachment0();
    
    frameBuffer.Unbind();
    
    return canny;
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