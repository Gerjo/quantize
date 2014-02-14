//
//  Model.cpp
//  Quantize
//
//  Created by Gerard Meier on 14/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "Model.h"
#include "Quantize.h"

    /// Construct a
    Model::Model() : _isuploaded(false), vbo{0}, texture(0) {

    }
    
    /// Determine if the vbo's are uplaoded.
    bool Model::isUpoaded() {
        return _isuploaded;
    }
    
    void Model::update(Quantize* q, const Matrix44& parent, const float dt) {
        if(isUpoaded()) {
            q->render(*this, parent);
        }
        
        Entity::update(q, parent * transform, dt);
    }
    
    void Model::upload() {
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
