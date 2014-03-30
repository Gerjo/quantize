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
#include "Model.h"
#include "Textures.h"
#include "Camera.h"
#include "Light.h"
#include "Collada.h"
#include "Model.h"
#include "KdTree.h"

using namespace Furiosity;
using std::string;

class Quantize {
    /// Application window width
    float width{256};
    
    /// Application window height
    float height{256};
    
    /// Raytrace program and shaders
    GLuint _programRaytracer{0};
    
    /// Vertices of the window.
    GLuint _vboRtVertices{0};
    
    /// Vertex array that embodies the above buffer.
    GLuint _vaoFrame{0};
    
    /// Texture that will contain all VertexData (traingles)
    GLuint _dataTexture{0};
    
    GLuint _glTimerQuery{0};
    
    /// Miscellaneous Uniforms
    GLint _attrRtPosition{-1};
    GLint _uniformRtWindowSize{-1};
    GLint _uniformRtRotation{-1};
    GLint _uniformRtTranslation{-1};
    GLint _uniformNumTriangles{-1};
    GLint _uniformTextures{-1};
    GLint _uniformDataTexture{-1};
    GLint _uniformTime{-1};
    GLint _uniformFrameCounter{-1};
    GLint _uniformPhotonTexture{-1};
    GLint _uniformNumPhotons{-1};
    
    GLint _uniformPhotonMapWidth{-1};
    GLint _uniformPhotonMapHeight{-1};
    
    /// Ray tracingproperties
    GLint _uniformN;
    GLint _uniformSigma;
    GLint _uniformRange;
    GLint _uniformJitter;
    GLint _uniformUseTexture;

    /// Light uniforms
    GLint _lightCount{-1};
    GLint _lightsPosition{-1};
    GLint _lightsDiffuse{-1};
    GLint _lightsSpecular{-1};
    GLint _lightsAmbiant{-1};
    
    /// Collection of models to render.
    std::vector<std::shared_ptr<Entity>> entities;
    
    /// Handle to a debug cube. Used to visualise physical light sources.
    std::shared_ptr<Model> cube;
    std::shared_ptr<Model> rectangle;
    std::shared_ptr<Model> triangle;
    std::shared_ptr<Model> model;
    
    /// A collection that will be uploaded as a texture to the GPU.
    std::vector<VertexData> scene;
    
    std::vector<Face> faces;
    
    std::vector<Photon> kdtree;
    
    /// Variables for statistical reporting
    double _lastLogTime{0}; // Time of last log print
    double _logInterval{10}; // Per seconds
    
    /// Inline struct, easier than prefixing all variables.
    struct Stats {
        double drawing;
        double uploadingDataTexture;
        double total;
        double uniforms;
        int frames;
        
        Stats() {
            reset();
        }
        
        void reset() {
            drawing = 0;
            uploadingDataTexture = 0;
            total = 0;
            uniforms = 0;
            frames = 0;
        }
    } stats;
    
    int _frameCounter{0};
    
    struct PhotonVariables {
        
        GLuint width{16};
        GLuint height{16};
        GLuint maxBounces{10};
        
        GLint uniformReadBuffer[3] = {0};

    
        GLuint program{0};
        GLuint fbo{0};
        GLuint textures[6] = {0};    // direction, position and meta outputs.
        GLuint renderBuffer{0};      // Ties all buffers together
        GLuint vbo{0};
        GLuint vao{0};
        GLint uniformWindowSize{-1};
        GLint attrPosition{-1};
        GLint uniformTextures{-1};
        
        GLint uniformPhotonMapWidth{-1};
        GLint uniformPhotonMapHeight{-1};

        GLuint photonTexture;
        
        GLint uniformData{-1};
        GLint unformTriangleCount{-1};
        GLint uniformLightCount{-1};
        GLint uniformLightsPosition{-1};
    } photon;
    
public:
    /// Raytracer Properties
    int n{1};
    float sigma{1.0};
    float range{2.0};
    int enableLights{1};
    bool enableJitter{true};
    bool useTexture{true};
    
    /// Camera
    Camera camera;
    
    /// Collection of light sources. Value semantics are used
    /// for better memory alignment. Array of structs idiom.
    std::vector<Light> lights;
    
    /// A shared instance, for easy access from the Objective-C GUI.
    static Quantize* getInstance() {
        static Quantize* instance = new Quantize();
        
        return instance;
    }
    

private:
    /// Default contructor, leaves the application in an undefined state. Use
    /// Quantize::initialize() to actually initialize.
    Quantize();
    
    /// Singleton based, no copy constructor.
    Quantize(const Quantize&) = delete;
    
    
    /// Singleton based, no assignment operator.
    Quantize& operator = (const Quantize& other) = delete;
    
    /// Statistical reporting to the console.
    void handleLogging();
    
    
public:
    /// Destructor
    ~Quantize();
    
    /// Load some decent looking models!
    void loadDemoScene();
    
    /// Initialize OpenGL and width/height dependent variables.
    void initialize(float width, float height);
    
    /// Load the raytracer shader program.
    void initializeRaytraceProgram();
    
    /// Load photon mapping shader program.
    void initializePhotonProgram();
    
    /// Entry point for the update and draw loops.
    ///
    /// @param Time elapsed since previous call to update.
    void update(float dt);
    
    
    /// Shoot... photons.
    void shootPhotons();
};

