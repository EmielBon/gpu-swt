//
//  IOGLResource.h
//  OGLTest
//
//  Created by Emiel Bon on 03-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include <GL/glew.h>

class IOGLResource
{
public:
    
    virtual GLuint GetHandle() const = 0;
    
    virtual void Dispose() = 0;
};
