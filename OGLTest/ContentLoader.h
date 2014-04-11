//
//  ContentLoader.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

class ContentLoader
{
public:
    
    template<class T>
    static Ptr<T> Load(const String &resourceName);
    
    template<class T>
    static T LoadV(const String &resourceName);
    
    static void SetContentPath(const String &contentPath);
    
    static String FileReadAll(const String& filePath);
    
    static void SetGraphicsDevice(GraphicsDevice *device);
    
    
public:
    
    static String ContentPath;
    
private:
    
    static GraphicsDevice *device;
};

inline void ContentLoader::SetGraphicsDevice(GraphicsDevice *d)
{
    device = d;
}
