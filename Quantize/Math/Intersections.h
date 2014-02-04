////////////////////////////////////////////////////////////////////////////////
//  Intersections.h
//  Furiosity
//
//  Created by Bojan Endrovski on 11/28/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "Vector2.h"
#include "Vector3.h"

namespace Furiosity
{
    /// Intersection between two line segments
    bool LineSegmentsInteresection(const Vector2& p,
                                   const Vector2& pr,
                                   const Vector2& q,
                                   const Vector2& qs,
                                   Vector2& res);

    /// Finds an intersection between a ray and a sphere
    /// if there is one.
    bool RayToSphere(const Vector3& rayOrigin,
                     const Vector3& rayDirection,
                     const Vector3& sphereOrigin,
                     float          sphereRadius,
                     Vector3&       res);
}


#endif
