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
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotateX = Matrix44::CreateRotateX(-mouse.y / 200.0f);
        Matrix44 _rotateY = _rotateX * Matrix44::CreateRotateY(mouse.x / 200.0f);
        Matrix44 _rotation = _rotateY * _rotateX;
        return _rotation * _translation;
        
        
        /*
        Matrix44::CreateRotateY(mouse.x/100.0f)
        * Matrix44::CreateRotateX(mouse.y/100.0f)
        * Matrix44::CreateTranslation(-2, -2, -5)
         */
    }
    
    void move(const Vector3& translation) {
        Matrix44 _rotateX = Matrix44::CreateRotateX(mouse.y / 100.0f);
        Matrix44 _rotateY = _rotateX * Matrix44::CreateRotateY(-mouse.x / 100.0f);
        Matrix44 _rotation = _rotateY * _rotateX;
        Vector3 _translation = _rotation * translation;
        position += _translation;
    }
    
    void onMove(const Vector2& location) {
        Vector2 _offset = Vector2(350.0f, 250.0f);
        mouse = location - _offset;
    }
    
    void onW() {
        move(Vector3(0, 0, 1));
    }
    
    void onA() {
        move(Vector3(1, 0, 0));
    }
    
    void onS() {
        move(Vector3(0, 0, -1));
    }
    
    void onD() {
        move(Vector3(-1, 0, 0));
    }
    
    void onQ() {
        
    }
    
    void onE() {
        
    }
    
    void onScroll(const Vector2& delta) {
        move(Vector3(0, 0, delta.y));
    }
    
    void onClick() {
        printf("%.2f %.2f \n", mouse.x, mouse.y);
    }
};