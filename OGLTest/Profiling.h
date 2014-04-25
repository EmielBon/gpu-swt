//
//  Profiling.h
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include <time.h>

#define PROFILING

#define now() clock()

static inline float GetTimeMsec(unsigned long clicks)
{
    return (clicks * 1000.0f) / CLOCKS_PER_SEC;
}

static inline void PrintTime(const String &name, unsigned long clicks)
{
    printf("%s (%.1fms)\n", name.c_str(), GetTimeMsec(clicks));
}

static inline void PrintTime2(const String &name, unsigned long clicks, unsigned long total)
{
    printf("%s: %.1fms (%.1f%%)\n", name.c_str(), GetTimeMsec(clicks), (clicks * 100.0f) / total);
}