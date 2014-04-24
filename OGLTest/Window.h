//
//  Window.h
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GraphicsDevice.h"
#include "types.h"

class Window
{
public:
    
    Window(int width, int height, const String &title);
    
    virtual ~Window() = default;
    
    void Show();
    
    virtual void Draw();
    
    void ResizeWithRect();
    
private:
    
    void RunMainLoop();
    
    static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    static void OnWindowResize(GLFWwindow* window, int width, int height);
    
protected:
    
    GLFWwindow* window;
};

