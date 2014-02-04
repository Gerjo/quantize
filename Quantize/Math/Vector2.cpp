////////////////////////////////////////////////////////////////////////////////
//  Vector2.cpp
//
//  Created by Bojan Endrovski on 23/09/2010.
//  Copyright 2010 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Frmath.h" // Gets its header form there

using namespace Furiosity;

Vector2 Vector2::RandomUnit()
{
    float rad = RandInRange(-Pi, Pi);
    //
    return Vector2( cosf(rad), sinf(rad));
}

/*
int Vector2:: Hash() const
{
    // Pick a weird coodrinate frame
    Vector2 origin(HalfPi, Pi);
    //
    union { float f; int i; } converter;
    
    converter.f = x;
    int xc = converter.i;
    //
    converter.f = y;
    int yc = converter.i;
    //
    converter.f = Distance(origin);
    int dc = converter.i;
    //
    return xc ^ yc ^ dc;
}
*/








// end
