//
//  Profiling.h
//  OGLTest
//
//  Created by Emiel Bon on 24-04-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"

#define PROFILING

#define now() std::chrono::high_resolution_clock::now()

static inline float GetTimeMsec(const TimeSpan &time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(time).count() / 1000000.0f;
}

static inline void PrintTime(const String &name, const TimeSpan &time)
{
    printf("%s (%.1fms)\n", name.c_str(), GetTimeMsec(time));
}

static inline void PrintTime2(const String &name, const TimeSpan &time, const TimeSpan &total)
{
    printf("%s: %.1fms (%.1f%%)\n", name.c_str(), GetTimeMsec(time), (time * 100.0f) / total);
}