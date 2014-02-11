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
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotation = computeRotation(false);
        return _rotation * _translation;
    }
    
    void update() {
        position += orientedTranslation(Vector3(control[LEFT] - control[RIGHT],
                                                control[DOWN] - control[UP],
                                                control[FORWARD] - control[BACKWARD]))
        * 0.1f; //Control speed.
        
        orientation.z += (control[CW] - control[CCW])
        * 0.04f; //Control speed.
    }
    
    void onMove(const Vector2 location) {
        Matrix44 _rollCompensation = Matrix44::CreateRotateZ(-1 * orientation.z);
        Vector3 _rotation = updateMouse(location);
        orientation += _rollCompensation * _rotation;
    }
    
    void onKey(char key) {
        switch (key) { //WASD Movement, RF for up/down, QE for roll.
            case 'w':
                control[FORWARD] = true;
                break;
            case 's':
                control[BACKWARD] = true;
                break;
            case 'a':
                control[LEFT] = true;
                break;
            case 'd':
                control[RIGHT] = true;
                break;
            case 'r':
                control[UP] = true;
                break;
            case 'f':
                control[DOWN] = true;
                break;
            case 'q':
                control[CCW] = true;
                break;
            case 'e':
                control[CW] = true;
                break;
            case 0x1b: //Escape exits.
                exit(0);
                break;
            default:
                printf("Registered KeyDown hex: %#0x \n",key);
                break;
        }
    }
    
    void onKeyUp(char key) {
        switch (key) { //WASD Movement, RF for up/down, QE for roll.
            case 'w':
                control[FORWARD] = false;
                break;
            case 's':
                control[BACKWARD] = false;
                break;
            case 'a':
                control[LEFT] = false;
                break;
            case 'd':
                control[RIGHT] = false;
                break;
            case 'r':
                control[UP] = false;
                break;
            case 'f':
                control[DOWN] = false;
                break;
            case 'q':
                control[CCW] = false;
                break;
            case 'e':
                control[CW] = false;
                break;
        }
    }
    
    void onScroll(const Vector2& delta) {
        position += orientedTranslation(Vector3(0, 0, delta.y));
    }
    
    void onClick() {
        printf("Registered click at: %.2f %.2f \n", mouse.x, mouse.y);
    }
    
private:
    Vector2 mouseOffset {350.0f, 250.0f};
    Vector2 mouse;
    
    bool control[8];
    enum controlKey{FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, CCW, CW};
    
    Vector3 updateMouse(const Vector2 location) {
        Vector2 _oldMouse = mouse;
        mouse = (location - mouseOffset) / 150.0f;
        Vector2 _d = mouse - _oldMouse;
        return Vector3(_d.x, _d.y, 0);
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
};