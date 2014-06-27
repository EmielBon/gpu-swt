//
//  ContentLoader.cpp
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "ContentLoader.h"
#include "Program.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureUtil.h"
#include <opencv2/highgui/highgui.hpp>

using namespace std;

String ContentLoader::ContentPath = "";

/*template<>
Ptr<VertexShader> ContentLoader::Load(const String &resourceName)
{
    String sourceText = FileReadAll(ContentPath + resourceName + ".vs");
    return New<VertexShader>(resourceName, sourceText);
}

template<>
Ptr<FragmentShader> ContentLoader::Load(const String &resourceName)
{
    String sourceText = FileReadAll(ContentPath + resourceName + ".fs");
    return New<FragmentShader>(resourceName, sourceText);
}*/

template<>
Ptr<Program> ContentLoader::Load(const String &vertexShaderSource, const String &fragmentShaderSource)
{
    List< Ptr<Shader> > shaders;
    
    auto vsSourceText = FileReadAll(ContentPath + vertexShaderSource   + ".vs");
    auto fsSourceText = FileReadAll(ContentPath + fragmentShaderSource + ".fs");
    
    auto vs = New<Shader>(vertexShaderSource,   vsSourceText, GL_VERTEX_SHADER);
    auto fs = New<Shader>(fragmentShaderSource, fsSourceText, GL_FRAGMENT_SHADER);
    
    //shaders.push_back(std::dynamic_pointer_cast<Shader>(vs));
    //shaders.push_back(std::dynamic_pointer_cast<Shader>(fs));
    
    shaders.push_back(vs);
    shaders.push_back(fs);
    
    return New<Program>(shaders);
}

template<>
Ptr<Program> ContentLoader::Load(const String &resourceName)
{
    return Load<Program>(resourceName, resourceName);
}

template<>
cv::Mat ContentLoader::LoadV(const String &resourceName)
{
    auto fileName = resourceName;
    
    if (resourceName.find_first_of(".") == String::npos)
        fileName += ".jpg";
    
    cv::Mat image = cvLoadImage((ContentPath + fileName).c_str());
    if (image.data == NULL)
        throw std::runtime_error("Error reading file");
    return image;
}

template<>
Ptr<Texture> ContentLoader::Load(const String &resourceName)
{
    return textureFromImage<Vector3>( LoadV<cv::Mat>(resourceName) );
}

String ContentLoader::FileReadAll(const String &filePath)
{
    //open file
    InputFileStream fs;
    fs.open(filePath.c_str(), ios::in | ios::binary);
    if(!fs.is_open()){
        throw runtime_error(String("Failed to open file: ") + filePath);
    }
    
    //read whole file into stringstream buffer
    StringStream buffer;
    buffer << fs.rdbuf();
    
    fs.close();
    
    return buffer.str();
}