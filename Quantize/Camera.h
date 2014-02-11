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
        Matrix44 _rotation = computeRotation(-1, 1);
        return _rotation * _translation;
    }
    
    Vector3 orientedTranslation(const Vector3& translation) {
        Matrix44 _rotation = computeRotation(-1, -1);
        return _rotation * translation;
    }
    
    Matrix44 computeRotation(int signX, int signY) {
        Matrix44 _rotateX = Matrix44::CreateRotateX(signX * mouse.y / 150.0f);
        Matrix44 _rotateY = Matrix44::CreateRotateY(signY * mouse.x / 150.0f);
        Matrix44 _roll = Matrix44::CreateRotateZ(roll / 4.0f);
        return _rotateX * _rotateY * _roll;
    }
    
    void onMove(const Vector2& location) {
        Vector2 _offset = Vector2(350.0f, 250.0f);
        mouse = location - _offset;
    }
    
    void onW() { //Move Forward
        position += orientedTranslation(Vector3(0, 0, 1));
    }
    
    void onA() { //Move Left
        position += orientedTranslation(Vector3(1, 0, 0));
    }
    
    void onS() { //Move Right
        position += orientedTranslation(Vector3(0, 0, -1));
    }
    
    void onD() { //Move Back
        position += orientedTranslation(Vector3(-1, 0, 0));
    }
    
    void onR() { //Move Up
        position += orientedTranslation(Vector3(0, -1, 0));
    }
    
    void onF() { //Move Down
        position += orientedTranslation(Vector3(0, 1, 0));
    }
    
    void onQ() { //Roll CCW
        roll--;
    }
    
    void onE() { //Roll CW
        roll++;
    }
    
    void onScroll(const Vector2& delta) {
        position += orientedTranslation(Vector3(0, 0, delta.y));
    }
    
    void onClick() {
        printf("%.2f %.2f \n", mouse.x, mouse.y);
    }
};