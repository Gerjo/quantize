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

using namespace Furiosity;

class Model {
    
public:
    GLuint vbo[2];
    
    std::vector<unsigned short> indices;
    std::vector<Vector3> vertices;

    Model() {
        
        vertices.push_back(Vector3(1, 0, 0));
        vertices.push_back(Vector3(0, 1, 0));
        vertices.push_back(Vector3(0, 0, 1));
    
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
    
        glGenBuffers(2, &vbo[0]);
        GLError();
        
        // Activate
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        // Activate
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
    }
    
};