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
#include "Camera.h"

#include "Collada.h"

using namespace Furiosity;
using std::string;

class Quantize {
public:
    /// Main shader program.
    GLuint _programMesh;
    
    /// Shader attributes.
    GLuint _attrPosition;
    GLuint _attrNormal;
    GLuint _attrColor;
    GLuint _attrUV;

    /// Shader uniforms.
    GLuint _uniformCamera;
    GLuint _uniformModelTransform;
    GLuint _uniformNormalTransform;
    GLuint _uniformSampler_1;

    /// Width width and height.
    float width;
    float height;
    
    /// Perspective projection.
    Matrix44 _projection;
    
    /// Camera
    Camera* camera = new Camera;

    /// Collection of models to render.
    std::vector<Model*> models;

    /// Framebuffer
    GLuint _fbo;
    GLuint _fboTexture;
    GLuint _renderBuffer;

    /// Post processing shader
    GLuint _programPost;
    GLuint _attrUvFBO;
    GLuint _uniformFboTexture;
    GLuint _uniformWindowSize;
    GLuint _vboFboVertices;
    
    static Quantize* getInstance() {
        if (instance == nullptr)
            instance = new Quantize();
        return instance;
    }
    
    ~Quantize() {
        glDeleteRenderbuffers(1, &_renderBuffer);
        glDeleteTextures(1, &_fboTexture);
        glDeleteFramebuffers(1, &_fbo);
        
        glDeleteProgram(_programPost);
        glDeleteProgram(_programMesh);
        
        glDeleteBuffers(1, &_vboFboVertices);
    }
    
    void initialize(float width, float height) {
        if(_programMesh != 0) {
            Exit("Program already initialized.");
        }
        
        this->width  = width;
        this->height = height;
        
        const GLubyte* version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        
        printf("Supported shader model: %s\n", version);
        
        // Let open GL deal with the z-index and order of rendering.
        glEnable(GL_DEPTH_TEST);
        
        // Hide faces not facing us.
        //glEnable(GL_CULL_FACE);
        
        //glDepthFunc(GL_ALWAYS);

        // Enable alpha layers
        glEnable (GL_BLEND);
        
        // Premultiplied alpha. (I think? I always confuse the two)
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Setup the mesh rendering shader programs
        initializeMeshProgram();
        
        // Setup the FBO, RBO and related shaders.
        initializePostProgram();
        
        // Affine coordinate transformation. We can do some scaling here,
        // e.g., map [-1,1] to [0, screensize] - though it makes more sense
        // to use a matrix (_projection) for that.
        glViewport(0, 0, width, height);
        
        // Usual perspective matrix
        _projection = Matrix44::CreatePerspective(
            3.14159268/2.5f,      // Field of view
            width/height,         // Aspect ratio
            1.0f,                 // near
            200.0f                // far
        );
        

        //for(Model* model : Collada::FromFile("models/AUSFB/ausfb.dae")) {
        for(Model* model : Collada::FromFile("models/P39 AIRACOBRA/p39.dae")) {
        //for(Model* model : Collada::FromFile("models/Earth/Earth.dae")) {
   
            // Create VBO (upload stuff to the GPU)
            model->upload();
            
            // Store internally
            models.push_back(model);
        }
    };
    
    void initializeMeshProgram() {
        // Programs contain shaders.
        _programMesh = glCreateProgram();

        // Prepare all shaders. These will exit on failure.
        GLuint vsh = CompileShader("shaders/basic.vsh");
        GLuint fsh = CompileShader("shaders/basic.fsh");
        
        // Attach vertex shader to program.
        glAttachShader(_programMesh, vsh);
        GLError();
        
        // Attach fragment shader to program.
        glAttachShader(_programMesh, fsh);
        GLError();

        glLinkProgram(_programMesh);
        GLValidateProgram(_programMesh);
        
        // Get a handle to shader attributes
        _attrPosition     = glGetAttribLocation(_programMesh, "position");
        _attrNormal       = glGetAttribLocation(_programMesh, "normal");
        _attrColor        = glGetAttribLocation(_programMesh, "color");
        _attrUV           = glGetAttribLocation(_programMesh, "uv");
        _uniformCamera    = glGetUniformLocation(_programMesh, "camera");
        _uniformModelTransform  = glGetUniformLocation(_programMesh, "modelTransform");
        _uniformNormalTransform = glGetUniformLocation(_programMesh, "normalTransform");
        _uniformSampler_1 = glGetUniformLocation(_programMesh, "sampler_1");
        GLError();

        // Remove the shaders, they are compiled and no longer required.
        glDetachShader(_programMesh, vsh);
        glDeleteShader(vsh);
        glDetachShader(_programMesh, fsh);
        glDeleteShader(fsh);
    }
    
    void initializePostProgram() {
        // We render to a texture, so let's create a texture.
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &_fboTexture);
        glBindTexture(GL_TEXTURE_2D, _fboTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLError();
        
        // Depth buffer  (Render Buffer Object)
        glGenRenderbuffers(1, &_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GLError();
        
        // Framebuffer to link everything together
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTexture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer);
        GLenum status;
        if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            Exit("glCheckFramebufferStatus: error %p.", status);
        }
      
        // Disable buffer, for now.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Prepare all shaders. These will exit on failure.
        GLuint postvsh = CompileShader("shaders/postp.vsh");
        GLuint postfsh = CompileShader("shaders/postp.fsh");
        
        GLint link_ok = 0;
        GLint validate_ok = 0;
        
        _programPost = glCreateProgram();
        glAttachShader(_programPost, postvsh);
        glAttachShader(_programPost, postfsh);
        glLinkProgram(_programPost);
        glGetProgramiv(_programPost, GL_LINK_STATUS, &link_ok);
        
        if ( ! link_ok) {
            Exit("Linking failed.");
        }
        
        glValidateProgram(_programPost);
        glGetProgramiv(_programPost, GL_VALIDATE_STATUS, &validate_ok);
        GLValidateProgram(_programPost);

        // Get a handle to the variables in the shader programs
        _attrUvFBO         = glGetAttribLocation(_programPost, "position");
        _uniformFboTexture = glGetUniformLocation(_programPost, "texture");
        _uniformWindowSize = glGetUniformLocation(_programPost, "windowSize");
        GLError();
        
        // The rectangle used to render onto, the UVs are derived from this.
        GLfloat fbo_vertices[] = {
            -1, -1,
             1, -1,
            -1,  1,
             1,  1,
        };
        
        glGenBuffers(1, &_vboFboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, _vboFboVertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Cleanup
        glDetachShader(_programPost, postvsh);
        glDeleteShader(postvsh);
        glDetachShader(_programPost, postfsh);
        glDeleteShader(postfsh);
    }
    
    
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
        glUniformMatrix4fv(_uniformModelTransform,  // Location
                            1,                      // Amount of matrices
                            false,                  // Require transpose
                            model.modelTransform.f  // Float array with values
        );
        GLError();
        
        // Set the perspective projection, once. This is shared among all models.
        glUniformMatrix3fv(_uniformNormalTransform,  // Location
                            1,                       // Amount of matrices
                            false,                   // Require transpose
                            model.normalTransform.f  // Float array with values
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
        
        glVertexAttribPointer(_attrNormal,                       // The attribute in the shader.
                            3,                                   // Number of "fields", in this case 3 floats X, Y & Z.
                            GL_FLOAT,                            // Data type
                            GL_FALSE,                            // Must these values be normalized? No thanks.
                            sizeof(VertexData),                  // Size of each structure
                            (void*) offsetof(VertexData, normal) // Offset
        );
        GLError();
        
        glEnableVertexAttribArray(_attrNormal);
        GLError();
        
        
        
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
            GL_UNSIGNED_INT,                  // Data type of indices
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
    
        // Enable framebuffer render target
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        GLFBError();
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Camera position (insert FPS code here).
        camera->update();
        Matrix44 transform = camera->transform();
        
        /*float distance = 14;
        transform = Matrix44::CreateLookAt(
            Vector3(-distance, distance, distance),
            Vector3(0, 0, 0),
            Vector3(1, 0, 0)
        );*/
        
        // Pre-multiply all projection related matrices. These are constant
        // terms.
        Matrix44 projection = _projection * transform;
        
        // Shader activate!
        glUseProgram(_programMesh);

        // Update the uniform.
        glUniformMatrix4fv(_uniformCamera,  // Location
                            1,              // Amount of matrices
                            false,          // Require transpose
                            projection.f   // Float array with values
        );

        for(Model* model : models) {
            model->update(dt);
            render(*model);
        }

        // Stop rendering to buffer.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLFBError();


        glClearColor(0.541, 0.361, 0.361, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(_programPost);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _fboTexture);
        glUniform1i(_uniformFboTexture, 0);
        glEnableVertexAttribArray(_attrUvFBO);
        GLError();
        
        glUniform2f(_uniformWindowSize, width, height);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboFboVertices);
        glVertexAttribPointer(
                _attrUvFBO,  // attribute
                2,                           // number of elements per vertex, here (x,y)
                GL_FLOAT,                    // the type of each element
                GL_FALSE,                    // take our values as-is
                0,                           // no extra data between each position
                0                            // offset of first element
        );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(_attrUvFBO);

        // Run draw calls.
        //glFlush();
        glSwapAPPLE();
    }
    
private:
    static Quantize* instance;
    
    Quantize()
    : _programMesh(0)
    , width(200)
    , height(100)
    , _attrPosition(0)
    , _attrNormal(0)
    , _attrColor(0)
    , _attrUV(0)
    , _uniformCamera(0)
    , _uniformModelTransform(0)
    , _uniformNormalTransform(0)
    , _uniformSampler_1(0)
    {
        
        
        
    }
};

