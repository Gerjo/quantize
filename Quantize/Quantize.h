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
#include "Light.h"

#include "Collada.h"

using namespace Furiosity;
using std::string;

class Quantize {
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
    
    /// Light uniforms
    GLuint _lightCount;
    GLuint _lightsPosition;
    GLuint _lightsDiffuse;
    GLuint _lightsSpecular;
    GLuint _lightsAmbiant;
    
    
    float width;
    float height;
    
    /// Perspective projection.
    Matrix44 _projection;
    

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
    

    /// Collection of models to render.
    std::vector<Model*> models;
    
    
public:
    /// Camera
    Camera* camera;
    
    /// Collection of light sources. Value semantics are used
    /// for better memory alignment. Array of structs idiom.
    std::vector<Light> lights;
    
    static Quantize* getInstance() {
        // This works, read the manual ;)
        static Quantize* instance = new Quantize();
        
        return instance;
    }
    

private:
    /// Default contructor, leaves the application in an undefined state. Use
    /// Quantize::initialize() to actually initialize.
    Quantize();
    
public:
    /// Destructor
    ~Quantize();
    
    /// Initialize OpenGL and width/height dependent variables.
    void initialize(float width, float height);
    
    /// Load the mesh (model) renderer program.
    void initializeMeshProgram();
    
    /// Load the post processing shader program.
    void initializePostProgram();
    
    /// Render a model.
    ///
    /// @param A model to render.
    void render(Model& model);
    
    /// Entry point for the update and draw loops.
    /// @param Time elapsed since previous call to update.
    void update(float dt);};

