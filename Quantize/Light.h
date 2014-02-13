//
//  Light.h
//  Quantize
//
//  Created by Gerard Meier on 13/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

using namespace Furiosity;


struct Color {
    //using byte = unsigned char;
    
    
    union {
        float v[4];
        
        struct {
            float r;
            float g;
            float b;
            float a;
        };
    };
    
    
    Color() : r{1}, g{1}, b{1}, a{1} {}
    
    /// Explicit copy ctor, unsure how well a union copies.
    Color(const Color& original) :
        r(original.r),
        g(original.g),
        b(original.b),
        a(original.a)
    {}
};

/// http://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Multiple_Lights

struct Light {
    /// Position in world
    Vector3 position;
    
    
    /// Colors
    Color ambient;
    Color diffuse;
    Color specular;
    
    /// Initialize this light with a usable state.
    Light()
    : position(10, 10, 10)
    {}
    
};
