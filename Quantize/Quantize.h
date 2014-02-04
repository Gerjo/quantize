////
//  Quantize.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <cstdio>

#include "Tools.h"
#include "Math/Vector3.h"
#include "Math/Matrix44.h"

#include "Model.h"

using namespace Furiosity;
using std::string;

class Quantize {
public:
    GLuint _program;
    GLuint _attrPosition;
    GLuint _uniformCamera;
    
    Model* model;
    
    Matrix44 _projection;

    Quantize() : _program(0), model(nullptr) {

        _projection.SetIndentity();
        
        _projection.SetScale(Vector3(
            0.5, 1, 1
        ));
    }
    
    void initialize() {
        if(_program != 0) {
            Exit("Program already initialized.");
        }
        
        _program = glCreateProgram();

        GLuint vsh = CompileShader("shaders/basic.vsh");
        GLuint fsh = CompileShader("shaders/basic.fsh");
        
        // Attach vertex shader to program.
        glAttachShader(_program, vsh);
        GLError();
        
        // Attach fragment shader to program.
        glAttachShader(_program, fsh);
        GLError();

        glLinkProgram(_program);
        GLValidateProgram(_program);
        
        // Get a handle to shader attributes
        _attrPosition  = glGetAttribLocation(_program, "position");
        _uniformCamera = glGetUniformLocation(_program, "camera");
        GLError();

        glDetachShader(_program, vsh);
        glDeleteShader(vsh);
        glDetachShader(_program, fsh);
        glDeleteShader(fsh);
        
        model = new Model();
    };
    
    
    void render(Model& model) {
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo[0]);
        GLError();
        
        glVertexAttribPointer(_attrPosition,                 // The attribute in the shader.
                            3,                               // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                        // Data type
                            GL_FALSE,                        // Must these values be normalized? No tanks.
                            sizeof(Vector3),                 // Size of each structure
                            0                                // Offset
        );
        GLError();
        
        glEnableVertexAttribArray(_attrPosition);
        GLError();
        
        // Bind vertex index buffer:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.vbo[1]);

        glDrawElements(
            GL_TRIANGLES,                     // GL primitive type
            (GLsizei) model.indices.size(),   // How many indices to draw
            GL_UNSIGNED_SHORT,                // Data type of indices
            0                                 // Offset
        );

        // Disable attributes, we've never done this before. Doing this makes sure
        // some global state doesn't leak into the next set of gl calls.
        glDisableVertexAttribArray(_attrPosition);
        GLError();

        // Unbind buffers:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GLError();
    }
    
    void update(float dt) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Shader activate!
        glUseProgram(_program);
        
        GLValidateProgram(_program);

        glUniformMatrix4fv(_uniformCamera,  // Location
                            1,              // Amount of matrices
                            0,              // Require transpose
                            _projection.f   // Float array with values
        );

        render(*model);

        // Run draw calls.
        //glFlush();
        glSwapAPPLE();
    }
    
};
