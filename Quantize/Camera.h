//
//  Camera.h
//  Quantize
//
//  Created by Meri van Tooren on 08/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <cstdio>

#include "Math/Vector3.h"
#include "Math/Matrix44.h"

#include "Tools.h"
#include "Parser.h"
#include "Model.h"
#include "Textures.h"

using namespace Furiosity;
using std::string;

class Camera {
public:
    /// Camera control trackers.
    Vector2 mouse;
    Vector3 position = Vector3(-2, -2, -10);
    float roll;
    Matrix44 orientation;
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotateX = Matrix44::CreateRotateX(-mouse.y / 100.0f);
        Matrix44 _rotateY = _rotateX * Matrix44::CreateRotateY(mouse.x / 100.0f);
        orientation = _rotateY * _rotateX;
        return orientation * _translation;
        
        
        /*
        Matrix44::CreateRotateY(mouse.x/100.0f)
        * Matrix44::CreateRotateX(mouse.y/100.0f)
        * Matrix44::CreateTranslation(-2, -2, -5)
         */
    }
    
    void onMove(const Vector2& location) {
        mouse = location;
    }
    
    void onScroll(const Vector2& delta) {
        Vector3 _translation = Vector3(0, 0, delta.y);
        _translation = orientation * _translation;
        position += _translation;
    }
    
    void onClick() {
        
    }
};