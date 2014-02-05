////
//  Quantize.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
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

class Quantize {
public:
    GLuint _program;
    GLuint _attrPosition;
    GLuint _attrNormal;
    GLuint _attrColor;
    GLuint _attrUV;

    GLuint _uniformCamera;
    GLuint _uniformTransform;
    GLuint _uniformSampler_1;

    float width;
    float height;
    
    Model* model;
    
    Matrix44 _projection;
    
    Vector3 position;
    
    Vector2 mouse;

    std::vector<Model*> models;

    Quantize()
        : _program(0)
        , width(200)
        , height(100)
        , _attrPosition(0)
        , _attrNormal(0)
        , _uniformCamera(0)
        , _uniformTransform(0)
        {
    }
    
    void onScroll(const Vector2& delta) {
    
    }
    
    void onMove(const Vector2& location) {
        mouse = location;
    }
    
    void initialize(float width, float height) {
        if(_program != 0) {
            Exit("Program already initialized.");
        }
        
        this->width  = width;
        this->height = height;
        
        printf("Window size: %d by %d\n", (int)width, (int)height);
        
        // Let open GL deal with the z-index and order of rendering.
        glEnable(GL_DEPTH_TEST);
        
        // Hide faces not facing us.
        glEnable(GL_CULL_FACE);
       
        // OpenGL space is normalized to [-1,1] for each dimension.
        // Setting this view port will create an affine mapping from
        // screen to opengl. TODO: let the projection matrix handle
        // this?
        glViewport(0, 0, width, height);
        
        // Usual perspective matrix
        _projection = Matrix44::CreatePerspective(
            3.14159268/2.0f,      // Field of view
            width/height,         // Aspect ratio
            0.01f,                // near
            1000.0f               // far
        );
        
        // Programs contain shaders.
        _program = glCreateProgram();

        // Prepare all shaders. These will exit on failure.
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
        _attrPosition     = glGetAttribLocation(_program, "position");
        _attrNormal       = glGetAttribLocation(_program, "normal");
        _attrColor        = glGetAttribLocation(_program, "color");
        _attrUV           = glGetAttribLocation(_program, "uv");
        _uniformCamera    = glGetUniformLocation(_program, "camera");
        _uniformTransform = glGetUniformLocation(_program, "transform");
        _uniformSampler_1 = glGetUniformLocation(_program, "sampler_1");
        GLError();

        // Remove the shaders, they are compiled and no longer required.
        glDetachShader(_program, vsh);
        glDeleteShader(vsh);
        glDetachShader(_program, fsh);
        glDeleteShader(fsh);
        
        // A mesh wrapper
        //model = new Model();
        //model->loadCube();
        //model->upload();
        
        for(Model* model : Parser::FromFile("models/tiger2.obj")) {
            model->texture = Textures::LoadPNG("models/" + model->group + ".png");
            //model->texture = Textures::LoadPNG("models/red.png");
            model->upload();
            models.push_back(model);
            
            
            
        }
    };
    
    
    /// Render a model.
    ///
    /// @param A model to render.
    void render(Model& model) {
        
        // Texture enabling
        glActiveTexture(GL_TEXTURE0);                   // Use texture 0
        glBindTexture(GL_TEXTURE_2D, model.texture);    // Work with this texture
        glUniform1i(_uniformSampler_1, 0);              // Set the sampler to tex 0
        GLError();
        
        // Set the perspective projection, once. This is shared among all models.
        glUniformMatrix4fv(_uniformTransform,  // Location
                            1,                 // Amount of matrices
                            false,             // Require transpose
                            model.transform.f  // Float array with values
        );
        GLError();
        
        // Load the vbo in global array buffer state
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo[0]);

        
        glVertexAttribPointer(_attrPosition,                       // The attribute in the shader.
                            3,                                     // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                              // Data type
                            GL_FALSE,                              // Must these values be normalized? No thanks.
                            sizeof(VertexData),                    // Size of each structure
                            (void*) offsetof(VertexData, position) // Offset
        );
        GLError();
         
        glEnableVertexAttribArray(_attrPosition);
        GLError();
        
        /*glVertexAttribPointer(_attrNormal,                       // The attribute in the shader.
                            3,                                   // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                            // Data type
                            GL_FALSE,                            // Must these values be normalized? No thanks.
                            sizeof(VertexData),                  // Size of each structure
                            (void*) offsetof(VertexData, normal) // Offset
        );
        GLError();
        
        glEnableVertexAttribArray(_attrNormal);
        GLError();*/
        
        
        
        glVertexAttribPointer(_attrColor,                      // The attribute in the shader.
                            4,                                  // Number of "fields", in this case 4: RGBA
                            GL_UNSIGNED_BYTE,                   // Data type
                            GL_TRUE,                            // Must these values be normalized? ja bitte
                            sizeof(VertexData),                 // Size of each structure
                            (void*) offsetof(VertexData, color) // Offset
        );
        GLError();
        
        glEnableVertexAttribArray(_attrColor);
        GLError();
        
        glVertexAttribPointer(_attrUV,                          // The attribute in the shader.
                            2,                                  // Number of "fields", in this case 2: U & V
                            GL_FLOAT,                           // Data type
                            GL_FALSE,                           // Must these values be normalized? nein
                            sizeof(VertexData),                 // Size of each structure
                            (void*) offsetof(VertexData, uv)    // Offset
        );
        GLError();
        
        glEnableVertexAttribArray(_attrUV);
        GLError();

        // Bind vertex index buffer.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.vbo[1]);
        GLError();
        
        //
        glDrawElements(
            GL_TRIANGLES,                     // GL primitive type
            (GLsizei) model.indices.size(),   // How many indices to draw
            GL_UNSIGNED_SHORT,                // Data type of indices
            0                                 // Offset
        );
        GLError();
        
        // Disable attributes. Doing this makes sure some global state
        // doesn't leak into the next set of gl calls.
        glDisableVertexAttribArray(_attrPosition);
        glDisableVertexAttribArray(_attrColor);
        GLError();

        // Unbind buffers.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GLError();
    }
    
    /// Entry point for the update and draw loops.
    /// @param Time elapsed since previous call to update.
    void update(float dt) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Camera position.
        Matrix44 transform =
            Matrix44::CreateTranslation(-2, -2, -5)
            * Matrix44::CreateRotateY(mouse.x/100.0f)
            * Matrix44::CreateRotateX(mouse.y/100.0f)
        
        ;
        
        // Pre-multiply all projection related matrices. These are constant
        // terms.
        Matrix44 projection = _projection * transform;
        
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Shader activate!
        glUseProgram(_program);

        // Update the uniform.
        glUniformMatrix4fv(_uniformCamera,  // Location
                            1,              // Amount of matrices
                            false,          // Require transpose
                            projection.f   // Float array with values
        );

        for(Model* model : models) {
            render(*model);
        }

        // Run draw calls.
        //glFlush();
        glSwapAPPLE();
    }
    
};
