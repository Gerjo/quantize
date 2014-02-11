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
    Vector3 position {-2, -2, -10};
    Vector3 orientation {0, 0, 0};
    Vector2 mouseOffset {350.0f, 250.0f};
    Vector2 mouse;
    bool control[8];
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotation = computeRotation(false);
        return _rotation * _translation;
    }
    
    void update() {
        position += orientedTranslation(Vector3(control[2] - control[3], control[5] - control[4], control[0] - control[1]))
                    * 0.1f; //Control speed.
        orientation.z += (control[7] - control[6])
                    * 0.04f; //Control speed.
    }
    
    Vector3 orientedTranslation(const Vector3 translation) {
        Matrix44 _rotation = computeRotation(true);
        return _rotation * translation;
    }
    
    Matrix44 computeRotation(bool inverse) {
        Matrix44 _rotateX = Matrix44::CreateRotateX((inverse?1:-1) * orientation.y);
        Matrix44 _rotateY = Matrix44::CreateRotateY((inverse?-1:1) * orientation.x);
        Matrix44 _roll = Matrix44::CreateRotateZ((inverse?-1:1) * orientation.z);
        if (!inverse)
            return _roll * _rotateX * _rotateY;
        else
            return _rotateY * _rotateX * _roll;
    }
    
    void onMove(const Vector2 location) {
        Vector2 _oldMouse = mouse;
        mouse = (location - mouseOffset) / 150.0f;
        Vector2 _d = mouse - _oldMouse;
        Matrix44 _rollCompensation = Matrix44::CreateRotateZ(-1 * orientation.z);
        Vector3 _rotation {_d.x, _d.y, 0};
        orientation += _rollCompensation * _rotation;
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
            case 0x1b: //Escape exits.
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