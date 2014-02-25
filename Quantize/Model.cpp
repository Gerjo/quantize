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
    Model::Model() : _isuploaded(false), vbo{0}, vao(0), texture(0) {

    }

    Model::~Model() {
        if(vao != 0) {
            glDeleteVertexArraysAPPLE(1, &vao);
            glDeleteBuffers(2, vbo);
        }
    }

    /// Determine if the vbo's are uplaoded.
    bool Model::isUpoaded() {
        return _isuploaded;
    }
    
    void Model::update(Quantize* q, const Matrix44& parent, const float dt) {
        
        if( ! vertices.empty()) {
            if( ! isUpoaded()) {
                upload(q);
            }
        
            //if(name == "Track") {
            if(isUpoaded()){
                q->render(*this, parent);
            }
            //}
        }
        
        Entity::update(q, parent * transform, dt);
    }
    
    void Model::upload(Quantize* quantize) {
        printf("uploading %s [vertices: %lu, indices: %lu]\n", name.c_str(), vertices.size(), indices.size());
        
        glGenVertexArrays(1, &vao);
        GLError();
        
        glBindVertexArray(vao);
        GLError();

        glGenBuffers(2, &vbo[0]);
        GLError();
        
        // Activate data buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        GLError();
        
/////////////////////////////////////////////////////////////////////////////////

         
        
        glVertexAttribPointer(quantize->_attrPosition,             // The attribute in the shader.
                            3,                                     // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                              // Data type
                            GL_FALSE,                              // Must these values be normalized? No thanks.
                            sizeof(VertexData),                    // Size of each structure
                            (void*) offsetof(VertexData, position) // Offset
        );
        GLError();
        glEnableVertexAttribArray(quantize->_attrPosition);
        GLError();
        
        
        glVertexAttribPointer(quantize->_attrNormal,             // The attribute in the shader.
                            3,                                   // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                            // Data type
                            GL_FALSE,                            // Must these values be normalized? No thanks.
                            sizeof(VertexData),                  // Size of each structure
                            (void*) offsetof(VertexData, normal) // Offset
        );
        GLError();
        glEnableVertexAttribArray(quantize->_attrNormal);
        GLError();
         
        
        glVertexAttribPointer(quantize->_attrSamplerIndex,        // The attribute in the shader.
                            1,                                    // Number of "fields", in this case 1: the index.
                            GL_UNSIGNED_INT,                      // Data type
                            GL_FALSE,                             // Must these values be normalized? No thanks.
                            sizeof(VertexData),                   // Size of each structure
                            (void*) offsetof(VertexData, sampler) // Offset
        );
        GLError();
        glEnableVertexAttribArray(quantize->_attrSamplerIndex);
        GLError();
        
         
        
        glVertexAttribPointer(quantize->_attrColor,             // The attribute in the shader.
                            4,                                  // Number of "fields", in this case 4: RGBA
                            GL_UNSIGNED_BYTE,                   // Data type
                            GL_TRUE,                            // Must these values be normalized? ja bitte
                            sizeof(VertexData),                 // Size of each structure
                            (void*) offsetof(VertexData, color) // Offset
        );
        GLError();
        glEnableVertexAttribArray(quantize->_attrColor);
        GLError();
        
        
        glVertexAttribPointer(quantize->_attrUV,                // The attribute in the shader.
                            2,                                  // Number of "fields", in this case 2: U & V
                            GL_FLOAT,                           // Data type
                            GL_FALSE,                           // Must these values be normalized? nein
                            sizeof(VertexData),                 // Size of each structure
                            (void*) offsetof(VertexData, uv)    // Offset
        );
        GLError();
        glEnableVertexAttribArray(quantize->_attrUV);
        GLError();
    
/////////////////////////////////////////////////////////////////////////////////
        
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        // Activate index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        _isuploaded = true;
    }
