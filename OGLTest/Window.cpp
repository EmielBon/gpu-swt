//
//  Window.cpp
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//
#include "Window.h"
#include "ContentLoader.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Program.h"

#include <iostream>
#include <stdexcept>

Window::Window(int width, int height, const String &title)
{
    if (glfwInit())
        printf("GLFW succesfully started!\n");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    
    if (!window)
        throw std::runtime_error("Error creating context");
    
    glfwSetKeyCallback(window, OnKeyPressed);
    glfwSetFramebufferSizeCallback(window, OnWindowResize);
    
    glfwMakeContextCurrent(window);
    
    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    
    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");
    
    glDisable(GL_CULL_FACE);
}

void Window::Show()
{
    RunMainLoop();
}

void Window::Draw()
{
    // unbind the index buffer
    IndexBuffer::BindDefault();
    // unbind the VAO
    VertexArray::BindDefault();
    // unbind the program
    Program::UseDefault();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::RunMainLoop()
{
    while (!glfwWindowShouldClose(window))
        Draw();
}

void Window::OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void Window::OnWindowResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}