//
//  Renderer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "RenderWindow.h"
#include "ContentLoader.h"
#include "Program.h"
#include "DrawableRect.h"
#include "Texture.h"
#include "SWTHelper.h"
#include "ImgProc.h"
#include "BoundingBox.h"

#include "types.h"
#include <stdio.h>
#include <assert.h>

RenderWindow* RenderWindow::instance = NULL;

RenderWindow::RenderWindow(int width, int height, const String &title)
    : base(width, height, title), device(GraphicsDevice)
{
    instance = this;
    
    ContentLoader::SetContentPath("/Users/emiel/Desktop/OGLTest/OGLTest/");
    rect1 = New<DrawableRect>(-1.0f, -1.0f, 1.0f, 1.0f);
    currentTextureIndex = 0;
    
    // Load the input image as a cv::Mat
    cv::Mat input = ContentLoader::LoadV<cv::Mat>("chep2"); AddTexture(input, "Input image");
    SetWindowSize(input.size(), {1024, 1024});

    // Load the shader program
    program = ContentLoader::Load<Program>("SimpleShader");
    
    List<BoundingBox> boundingBoxes = SWTHelper::StrokeWidthTransform(input);
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
    device.VertexBuffer = rect.VertexBuffer;
    device.IndexBuffer  = rect.IndexBuffer;
    
    auto &texture = *textures[currentTextureIndex];
    
    program->Apply();
    program->Uniforms["Texture"].SetValue(texture);
    program->Uniforms["Channels"].SetValue(texture.GetColorChannels());
    
    device.DrawPrimitives(PrimitiveType::TriangleList);
}

void RenderWindow::Draw()
{
    static bool keyPressed = false;
    
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
    if (!glfwGetKey(window, GLFW_KEY_RIGHT) && !glfwGetKey(window, GLFW_KEY_LEFT))
    {
        keyPressed = false;
    }
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawRect(*rect1);
    
    base::Draw();
}

void RenderWindow::AddTexture(const cv::Mat &mat, const String &descriptor)
{
    textures.push_back( New<Texture>(mat) );
    textureDescriptors.push_back(descriptor);
}