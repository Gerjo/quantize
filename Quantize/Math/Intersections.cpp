////////////////////////////////////////////////////////////////////////////////
//  Intersections.cpp
//
//  Created by Bojan Endrovski on 11/29/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Intersections.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// LineSegmentsInteresection
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::LineSegmentsInteresection(const Vector2 &p,
                                          const Vector2 &pr,
                                          const Vector2 &q,
                                          const Vector2 &qs,
                                          Vector2 &res)
{
    // Get R and S
    Vector2 r = pr - p;
    Vector2 s = qs - q;
    
    // Get the fake 2D cross product
    float rs = r.Cross(s);
    if(rs == 0.0f)  return false;   // TODO: Introduce some tolerance
    
    // Get the params
    Vector2 qp = q - p;
    float t = qp.Cross(s) / rs;
    float u = qp.Cross(r) / rs;
    
    // Check if the params are in the range
    if(t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        res = p + r * t;
        return true;
    }
    else
        return false;
}


////////////////////////////////////////////////////////////////////////////////
// RayToSpere
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::RayToSphere(const Vector3& origin,
                            const Vector3& direction,
                            const Vector3& center,
                            float          radius,
                            Vector3&       res)
{
    // Implementation straight from the vector implicit formulation for a sphere: |c-x|=r
    // and the parametric formulation of a line: p(t)=p0+v*t
    Vector3 d = direction;
    d.Normalize();
    //
    // coefficient a = 1
    float b = 2 * (d.Dot(origin) - d.Dot(center));
    float c =   origin.SquareMagnitude() + center.SquareMagnitude() - (radius*radius)
                - 2 * origin.Dot(center);
    float discriminant = b * b -  4 * c;
    //
    if(discriminant < 0.0f)	// Early out
        return false;
    //
    float t0 = (-b - sqrtf(discriminant)) / 2;
    float t1 = (-b + sqrtf(discriminant)) / 2;
    //
    float t = t0;
    if( (t1 > 0 && t1 < t) || t < 0.0f )
        t = t1;
    //
    if(t <= 0.0f)			// Early out
        return false;
    //
    res = origin + d * t;
    return true;
}


// end