//
//  Renderer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include <assert.h>
#include <stdio.h>

#include "BoundingBox.h"
#include "ContentLoader.h"
#include "DrawableRect.h"
#include "ImgProc.h"
#include "Program.h"
#include "RenderWindow.h"
#include "SWTHelper.h"
#include "SWTHelperGPU.h"
#include "Texture.h"
#include "types.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GLError.h"
#include "FrameBuffer.h"

RenderWindow* RenderWindow::instance = nullptr;

RenderWindow::RenderWindow(int width, int height, const String &title)
    : base(width, height, title)
{
    instance = this;
    
    ContentLoader::ContentPath = "/Users/emiel/Desktop/OGLTest/OGLTest/";
    currentTextureIndex = 0;
    
    // Load the input image as a cv::Mat
    cv::Mat input = ContentLoader::LoadV<cv::Mat>("sign800x600"); AddTexture(input, "Input image");
    SetWindowSize(input.size(), {1024, 1024});

    check_gl_error();
    
    rect1 = New<DrawableRect>(-1, 1, 1, -1);
    
    // Load the shader program
    List< Ptr<Shader> > shaders;
    
    auto vs = ContentLoader::Load<VertexShader>("Trivial");
    auto fs = ContentLoader::Load<FragmentShader>("Normal");
    
    shaders.push_back(std::dynamic_pointer_cast<Shader>(vs));
    shaders.push_back(std::dynamic_pointer_cast<Shader>(fs));
    
    program = New<Program>(shaders);
    
    List<BoundingBox> boundingBoxes = SWTHelperGPU::StrokeWidthTransform(input);
    cv::Mat output = ImgProc::DrawBoundingBoxes(input, boundingBoxes, {0, 255, 255, 255});
    AddTexture(output, "Detected text regions");
}

void RenderWindow::SetWindowSize(const cv::Size &size, const cv::Size &max)
{
    float aspectRatio = size.width / (float)size.height;
    int width = std::min(size.width, max.width);
    //int height = std::min(size.height, max.height);
    
    glfwSetWindowSize(window, width, width / aspectRatio);
}

void RenderWindow::DrawRect(const DrawableRect &rect)
{
    GraphicsDevice::SetBuffers(rect.VertexBuffer, rect.IndexBuffer);
    
    auto &texture = *textures[currentTextureIndex];
    
    program->Use();
    program->Uniforms["Texture"].SetValue(texture);
    program->Uniforms["Channels"].SetValue(texture.GetColorChannels());
    
    GraphicsDevice::DrawPrimitives(PrimitiveType::Triangles);
}

void RenderWindow::Draw()
{
    check_gl_error();
    
    static bool keyPressed = false; // to make it draw the first time
    
    if (glfwGetKey(window, GLFW_KEY_RIGHT) && !keyPressed)
    {
        currentTextureIndex = (currentTextureIndex + 1) % textures.size();
        keyPressed = true;
        glfwSetWindowTitle(window, textureDescriptors[currentTextureIndex].c_str());
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) && !keyPressed)
    {
        currentTextureIndex--;
        if (currentTextureIndex < 0)
            currentTextureIndex += textures.size();
        keyPressed = true;
        glfwSetWindowTitle(window, textureDescriptors[currentTextureIndex].c_str());
    }
    if (keyPressed)
    {
        FrameBuffer::BindDefault();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        DrawRect(*rect1);
    }
    if (!glfwGetKey(window, GLFW_KEY_RIGHT) && !glfwGetKey(window, GLFW_KEY_LEFT))
    {
        keyPressed = false;
    }
    
    base::Draw();
}

void RenderWindow::AddTexture(const cv::Mat &mat, const String &descriptor)
{
    AddTexture(New<Texture>(mat), descriptor);
}

void RenderWindow::AddTexture(Ptr<Texture> texture, const String &descriptor)
{
    textures.push_back(texture);
    textureDescriptors.push_back(descriptor);
}