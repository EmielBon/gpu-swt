//
//  IOGLBindableResource.h
//  OGLTest
//
//  Created by Emiel Bon on 20-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "IOGLResource.h"

class IOGLBindableResource : public IOGLResource
{
public:
    
    virtual void Bind() = 0;
    
    virtual void Unbind() = 0;
};
