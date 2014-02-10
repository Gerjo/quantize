//
//  Model.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Tools.h"
#include <vector>
#include "Math/Vector3.h"
#include "Math/Matrix44.h"
#include "Math/Matrix33.h"

using namespace Furiosity;

struct VertexData {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
    unsigned char color[4] = {(unsigned char)0, 255, 0, 0};
    
    VertexData(Vector3 position,  Vector3 normal = Vector3(0, 0, 0), Vector2 uv = Vector2(1, 1))
        : position(position)
        , normal(normal)
        , uv(uv) {
            // codes
    }
    
    VertexData(float x,float y,float z,float a,float b,float c, float u = 1, float v = 1)
        : position(x, y, z)
        , normal(a, b, c)
        , uv(u, v) {
            // codes
    }};

class Model {
private:
    bool _isuploaded;
    
    float angle;
    
public:
    GLuint vbo[2];
    
    Matrix44 modelTransform;
    Matrix33 normalTransform;
    
    std::vector<unsigned int> indices;
    std::vector<VertexData> vertices;

    // Identifiers as specified in the obj file.
    std::string material;
    std::string group;

    GLuint texture;

    Model() : _isuploaded(false), angle(0), vbo{0}, texture(0) {
        modelTransform.SetIndentity();
        normalTransform.SetIdentity();
    }
    
    bool isUpoaded() {
        return _isuploaded;
    }
    
    void update(const float dt) {
        angle += 0.003;
        
        modelTransform  = Matrix44::CreateRotate(angle, 0, 1, 0);
        normalTransform = modelTransform.GetMatrix33();
    }
    
    void upload() {
        glGenBuffers(2, &vbo[0]);
        GLError();
        
        // Activate
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        // Activate
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        _isuploaded = true;
    }
};