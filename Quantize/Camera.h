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
    Vector3 position = Vector3(-2, -2, -10);
    Vector2 mouse;
    float roll;
    bool control[10];
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotation = computeRotation(-1, 1, true);
        return _rotation * _translation;
    }
    
    void update() {
        position += orientedTranslation(Vector3(control[2] - control[3], control[5] - control[4], control[0] - control[1]))
                    * 0.1f; //Control speed.
        roll += (control[6] - control[7])
                    * 0.2f; //Control speed.
    }
    
    Vector3 orientedTranslation(const Vector3& translation) {
        Matrix44 _rotation = computeRotation(1, -1, false);
        return _rotation * translation;
    }
    
    Matrix44 computeRotation(int signX, int signY, bool flag) {
        Matrix44 _rotateX = Matrix44::CreateRotateX(signX * mouse.y / 150.0f);
        Matrix44 _rotateY = Matrix44::CreateRotateY(signY * mouse.x / 150.0f);
        Matrix44 _roll = Matrix44::CreateRotateZ(roll);
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
                control[0] = true;
                break;
            case 's':
                control[1] = true;
                break;
            case 'a':
                control[2] = true;
                break;
            case 'd':
                control[3] = true;
                break;
            case 'r':
                control[4] = true;
                break;
            case 'f':
                control[5] = true;
                break;
            case 'q':
                control[6] = true;
                break;
            case 'e':
                control[7] = true;
                break;
            case 0xd: //Return exits.
                exit(0);
                break;
            default:
                printf("Registered KeyDown hex: %#0x \n",key);
                break;
        }
    }
    
    void onKeyDown(char key) {
        switch (key) { //WASD Movement, RF for up/down, QE for roll.
            case 'w':
                control[0] = false;
                break;
            case 's':
                control[1] = false;
                break;
            case 'a':
                control[2] = false;
                break;
            case 'd':
                control[3] = false;
                break;
            case 'r':
                control[4] = false;
                break;
            case 'f':
                control[5] = false;
                break;
            case 'q':
                control[6] = false;
                break;
            case 'e':
                control[7] = false;
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