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

#include "Model.h"

using namespace Furiosity;
using std::string;

class Quantize {
public:
    
    /// Light uniforms
    GLuint _lightCount;
    GLuint _lightsPosition;
    GLuint _lightsDiffuse;
    GLuint _lightsSpecular;
    GLuint _lightsAmbiant;
    
    
    float width;
    float height;
    
    /// Ray tracer shader (todo: zero initialize)
    GLint  _programRaytracer{0};
    GLuint _vboRtVertices;
    GLint  _attrRtPosition;
    GLint  _uniformRtWindowSize;
    GLint  _uniformRtRotation;
    GLint  _uniformRtTranslation;
    GLint  _uniformNumTriangles;
    GLint  _uniformTextures;
    GLint  _uniformDataTexture;
    GLuint _vaoFrame;
    GLuint _dataTexture;
    
    /// Collection of models to render.
    std::vector<std::shared_ptr<Entity>> entities;
    
    /// Handle to a debug cube. Used to visualise physical light sources.
    std::shared_ptr<Model> cube;
    std::shared_ptr<Model> rectangle;
    std::shared_ptr<Model> triangle;
    std::shared_ptr<Model> model;
    
    std::vector<VertexData> scene;
    
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
    
    /// Load some decent looking models!
    void loadDemoScene();
    
    /// Initialize OpenGL and width/height dependent variables.
    void initialize(float width, float height);
    
    /// Load the raytracer shader program.
    void initializeRaytraceProgram();
    
    /// Entry point for the update and draw loops.
    ///
    /// @param Time elapsed since previous call to update.
    void update(float dt);
};

