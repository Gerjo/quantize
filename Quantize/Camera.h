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
        Matrix44 _rotation = computeRotation(-1, 1, true);
        return _rotation * _translation;
    }
    
    Vector3 orientedTranslation(const Vector3& translation) {
        Matrix44 _rotation = computeRotation(1, -1, false);
        return _rotation * translation;
    }
    
    Matrix44 computeRotation(int signX, int signY, bool flag) {
        Matrix44 _rotateX = Matrix44::CreateRotateX(signX * mouse.y / 150.0f);
        Matrix44 _rotateY = Matrix44::CreateRotateY(signY * mouse.x / 150.0f);
        Matrix44 _roll = Matrix44::CreateRotateZ(roll / 4.0f);
        if (flag)
            return _rotateX * _rotateY * _roll;
        else
            return _roll * _rotateY * _rotateX;
    }
    
    void onMove(const Vector2& location) {
        Vector2 _offset = Vector2(350.0f, 250.0f);
        mouse = location - _offset;
    }
    
    void onKey(char key) {
        switch (key) { //WASD Movement, RF for up/down, QE for roll.
            case 'w':
                position += orientedTranslation(Vector3(0, 0, 1));
                break;
            case 'a':
                position += orientedTranslation(Vector3(1, 0, 0));
                break;
            case 's':
                position += orientedTranslation(Vector3(0, 0, -1));
                break;
            case 'd':
                position += orientedTranslation(Vector3(-1, 0, 0));
                break;
            case 'r':
                position += orientedTranslation(Vector3(0, -1, 0));
                break;
            case 'f':
                position += orientedTranslation(Vector3(0, 1, 0));
                break;
            case 'q':
                roll--;
                break;
            case 'e':
                roll++;
                break;
            case 0xd: //Return exits.
                exit(0);
                break;
            default:
                printf("Registered KeyDown hex: %#0x \n",key);
                break;
        }
    }
    
    void onScroll(const Vector2& delta) {
        position += orientedTranslation(Vector3(0, 0, delta.y));
    }
    
    void onClick() {
        printf("Registered click at: %.2f %.2f \n", mouse.x, mouse.y);
    }
};