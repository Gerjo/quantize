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

class Camera {
public:
    Vector3 position {-2, -2, -10};
    Vector3 orientation {0, 0, 0};
    
    float moveSpeed {0.3f};
    float mouseSpeed {0.007f};
    float rollSpeed {0.04f};
    
    bool locked {true};
    
    Camera() {
        
    }
    
    Matrix44 transform() {
        if (!locked) {
        Matrix44 _translation = Matrix44::CreateTranslation(position.x, position.y, position.z);
        Matrix44 _rotation = computeRotation();
        return _rotation * _translation;
        }
        else return defaultLookat();
    }
    
    void update() {
        position += orientedTranslation(Vector3(control[LEFT] - control[RIGHT],
                                                control[DOWN] - control[UP],
                                                control[FORWARD] - control[BACKWARD])) * moveSpeed;
        orientation.z += (control[CW] - control[CCW]) * rollSpeed;
    }
    
    void onMove(const Vector2& location) {
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
                printf("Registered KeyDown hex: %#0x\n",key);
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
        printf("Registered click at: %.2f %.2f\n", mouse.x, mouse.y);
    }
    
private:
    Vector2 mouse;
    Vector2 mouseOffset {350.0f, 250.0f};
    
    bool control[8];
    enum controlKey{FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, CCW, CW};
    
    Matrix44 defaultLookat() {
        float _distance = 14;
        return Matrix44::CreateLookAt(
                                      Vector3(-_distance, _distance, _distance),
                                      Vector3(0, 0, 0),
                                      Vector3(0, 1, 0)
                                      );
    }
    
    Vector3 updateMouse(const Vector2& location) {
        Vector2 _oldMouse = mouse;
        mouse = (location - mouseOffset) * mouseSpeed;
        Vector2 _d = mouse - _oldMouse;
        return Vector3(_d.x, _d.y, 0);
    }
    
    Vector3 orientedTranslation(const Vector3& translation) {
        Matrix44 _rotation = computeInverseRotation();
        return _rotation * translation;
    }
    
    Matrix44 computeRotation() {
        Matrix44 _rotateX = Matrix44::CreateRotateX(-1 * orientation.y);
        Matrix44 _rotateY = Matrix44::CreateRotateY(orientation.x);
        Matrix44 _roll = Matrix44::CreateRotateZ(orientation.z);
        return _roll * _rotateX * _rotateY;
    }
    
    Matrix44 computeInverseRotation() {
        Matrix44 _rotateX = Matrix44::CreateRotateX(1 * orientation.y);
        Matrix44 _rotateY = Matrix44::CreateRotateY(-1 * orientation.x);
        Matrix44 _roll = Matrix44::CreateRotateZ(-1 * orientation.z);
        return _rotateY * _rotateX * _roll;
    }
};