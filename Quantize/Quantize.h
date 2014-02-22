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
    /// Main shader program.
    GLuint _programMesh;
    
    /// Shader attributes.
    GLuint _attrPosition;
    GLuint _attrNormal;
    GLuint _attrColor;
    GLuint _attrUV;
    GLuint _attrSamplerIndex;
    
    /// Shader uniforms.
    GLint _uniformCamera;
    GLint _uniformModelTransform;
    GLint _uniformNormalTransform;
    //GLuint _uniformSampler_1;
    GLint _uniformSamplers[15];
    
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
    GLint  _attrUvFBO;
    GLint  _uniformFboTexture;
    GLint  _uniformWindowSize;
    GLuint _vboFboVertices;
    GLint  _uniformKernelType;
    GLint  _uniformKernelLerp;
    
    /// Ray tracer shader (todo: zero initialize)
    GLint  _programRaytracer;
    GLuint _vboRtVertices;
    GLint  _attrRtPosition;
    GLint  _uniformRtWindowSize;
    GLint  _uniformEdgeA;
    GLint  _uniformEdgeB;
    GLint  _uniformRtRotation;
    GLint  _uniformRtTranslation;
    GLint  _uniformEdgeC;
    GLint  _uniformNumTriangles;
    
    /// Collection of models to render.
    std::vector<std::shared_ptr<Entity>> entities;
    
    /// Handle to a debug cube. Used to visualise physical light sources.
    std::shared_ptr<Model> cube;
    std::shared_ptr<Model> rectangle;
    std::shared_ptr<Model> triangle;
    
public:
    /// Type of convolution kernel.
    /// 0:        kernel = identity;
    /// 1:        kernel = blur1;
    /// 2:        kernel = blur2;
    /// 3:        kernel = edge1;
    /// 4:        kernel = edge2;
    /// 5:        kernel = edge3;
    /// 6:        kernel = sharpen;
    int kernelType;
    
    /// Blend value of the convolution kernel. 0 = kernel only, 1 = original only.
    float kernelLerp;
    

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
    
    /// Load the mesh (model) renderer program.
    void initializeMeshProgram();
    
    /// Load the post processing shader program.
    void initializePostProgram();
    
    /// Load the raytracer shader program.
    void initializeRaytraceProgram();
    
    /// Render a model.
    ///
    /// @param A model to render.
    /// @param An additional transform.
    void render(Model& model, const Matrix44& transform = Matrix44());
   
    /// Entry point for the update and draw loops.
    ///
    /// @param Time elapsed since previous call to update.
    void update(float dt);
};

