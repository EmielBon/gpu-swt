//
//  Renderer.h
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Window.h"
#include "types.h"

class RenderWindow : public Window
{
private:
    
    typedef Window base;
    
public:
    
    RenderWindow(int width, int height, const String &title);
    
    static RenderWindow& Instance();
    
    void DrawRect(const DrawableRect &rect);
    
    void Draw();
 
    void AddTexture(const cv::Mat &mat, const String &descriptor = "");
    
    void AddTexture(Ptr<Texture> texture, const String &descriptor = "");
    
    void AddFrameBufferSnapshot(const String &descriptor = "");
    
private:
    
    void SetWindowSize(const cv::Size &size, const cv::Size &max);
    
private:
    
    Ptr<Program>       program;
    Ptr<DrawableRect>  rect1;
    List<Ptr<Texture>> textures;
    List<String>       textureDescriptors;
    int                currentTextureIndex;
    
    static RenderWindow* instance;
};

inline RenderWindow& RenderWindow::Instance()
{
    return *instance;
}