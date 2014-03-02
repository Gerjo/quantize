////
//  Quantize.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "Quantize.h"
#include "Entity.h"
#include "Textures.h"

Quantize::Quantize() : _lastLogTime(GetTiming()) {
    /*
    Light light;
    light.position.x = 15.0f;
    light.position.y = 5.0f;
    light.position.z = 20.0f;
    
    for(size_t i = 0; i < 4; ++i) {
        light.ambient.v[i]  = 0.2f;
        light.diffuse.v[i]  = 0.2f;
        light.specular.v[i] = 0.0f;
    }
  
    light.diffuse.r = 0;
    light.diffuse.g = 0;
    light.diffuse.b = 2;
    lights.push_back(light);
    */
        
    /*light.diffuse.r = 1;
    light.diffuse.g = 0;
    light.diffuse.b = 0;
    light.position.z = 40.0f;
    lights.push_back(light);
    
    light.diffuse.r = 0;
    light.diffuse.g = 1;
    light.diffuse.b = 0;
    light.position.z = 15.0f;
    light.position.x = 40.0f;
    lights.push_back(light);
    
    light.diffuse.r = 0;
    light.diffuse.g = 1;
    light.diffuse.b = 1;
    light.position.z = 15.0f;
    light.position.x = -30.0f;
    lights.push_back(light);*/
}

Quantize::~Quantize() {
    glDeleteProgram(_programRaytracer);
    glDeleteVertexArrays(1, &_vaoFrame);
    glDeleteBuffers(1, &_vboRtVertices);
}

void Quantize::loadDemoScene() {
    //Textures::LoadPNG("models/tmp/red.png");
    
    cube      = Collada::FromFile("models/Magey/Magey.dae");
    rectangle = Collada::FromFile("models/Plane/checkerboard3.dae");
    triangle  = Collada::FromFile("models/Plane/triangle.dae");
    
    model     = Collada::FromFile("models/Rock1/Rock1.dae");
    
    entities.push_back(Collada::FromFile("models/cube.dae"));

    // Custom designed cube by Master Mergon with textures forged from the
    // lava of Mount Doom enriched with tiny ring-like particles.
    scene.insert(scene.end(), ((Model*)cube->sub[0].get())->vertices.begin(), ((Model*)cube->sub[0].get())->vertices.end());
    
    // A checkerboard of checkerboards.
    float scale = 25;
    for(int x = 0; x < 1; ++x) {
        for(int y = 0; y < 1; ++y) {
            Matrix44 t = Matrix44::CreateRotateX(3.14 / 2) * Matrix44::CreateTranslation(x * scale * 2, y * scale * 2, -1) * Matrix44::CreateScale(scale);
            
            for(VertexData d : ((Model*)rectangle->sub[0].get())->vertices) {
                d.position = t *  d.position;
            
                scene.push_back(d);
            }
        }
    }
}


void Quantize::initialize(float width, float height) {
    if(_programRaytracer != 0) {
        Exit("Program already initialized.");
    }
    
    this->width  = width;
    this->height = height;
    
    int result = 0;
    printf("Supported shader model: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &result);
    printf("Maximum vertex texture samplers: %d\n", result);
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &result);
    printf("Maximum fragment texture samplers: %d\n", result);
    
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result);
    printf("Maximum texture size: %dx%dpx\n", result, result);

    printf("\n\n");
    
    // Let open GL deal with the z-index and order of rendering.
    glEnable(GL_DEPTH_TEST);
    
    // Hide faces not facing us.
    //glEnable(GL_CULL_FACE);
    
    //glDepthFunc(GL_ALWAYS);

    // Enable alpha layers
    glEnable (GL_BLEND);
    
    // Premultiplied alpha. (I think? I always confuse the two)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
      
    // Affine coordinate transformation. We can do some scaling here,
    // e.g., map [-1,1] to [0, screensize] - though it makes more sense
    // to use a matrix (_projection) for that.
    glViewport(0, 0, width, height);
        
    // Experimental raytacer
    initializeRaytraceProgram();
    
    loadDemoScene();
};


void Quantize::initializeRaytraceProgram() {
    // Prepare all shaders. These will exit on failure.
    GLuint vsh = CompileShader("shaders/raytracer.vsh");
    GLuint fsh = CompileShader("shaders/raytracer.fsh");
    
    // Create shader program
    _programRaytracer = glCreateProgram();
    glAttachShader(_programRaytracer, vsh); GLError();
    printf("Raytrace vertex - compiled.\n");
    
    glAttachShader(_programRaytracer, fsh); GLError();
    printf("Raytrace fragment - compiled.\n");
    
    glLinkProgram(_programRaytracer);
    //GLValidateProgram(_programRaytracer);
    GLError();

    _uniformRtWindowSize = glGetUniformLocation(_programRaytracer, "windowSize");
    _attrRtPosition = glGetAttribLocation(_programRaytracer, "vertexPosition");
    _uniformNumTriangles  = glGetUniformLocation(_programRaytracer, "numTriangles");
    _uniformRtRotation    = glGetUniformLocation(_programRaytracer, "rotation");
    _uniformRtTranslation = glGetUniformLocation(_programRaytracer, "translation");
    
    _uniformTextures = glGetUniformLocation(_programRaytracer, "textures");
    _uniformDataTexture = glGetUniformLocation(_programRaytracer, "zdata");
    GLError();
    
    _lightCount     = glGetUniformLocation(_programRaytracer, "lightCount");
    _lightsPosition = glGetUniformLocation(_programRaytracer, "lightsPosition");
    _lightsDiffuse  = glGetUniformLocation(_programRaytracer, "lightsDiffuse");
    _lightsSpecular = glGetUniformLocation(_programRaytracer, "lightsSpecular");
    _lightsAmbiant  = glGetUniformLocation(_programRaytracer, "lightsAmbiant");
    GLError();
    
    // The rectangle used as canvas where ray are shot from.
    GLfloat vertices[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };
    
    glGenVertexArrays(1, &_vaoFrame);
    glBindVertexArray(_vaoFrame);
    
    glGenBuffers(1, &_vboRtVertices);
    glBindBuffer(GL_ARRAY_BUFFER, _vboRtVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLError();

    glVertexAttribPointer(
            _attrRtPosition,             // attribute
            2,                           // number of elements per vertex, here (x,y)
            GL_FLOAT,                    // the type of each element
            GL_FALSE,                    // take our values as-is
            0,                           // no extra data between each position
            0                            // offset of first element
    );
    GLError();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Cleanup
    glDetachShader(_programRaytracer, vsh);     GLError();
    glDeleteShader(vsh);                        GLError();
    glDetachShader(_programRaytracer, fsh);     GLError();
    glDeleteShader(fsh);                        GLError();

    // Data texture to hold vertices
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_dataTexture);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLError();
    
    glGenQueries(1, &_glTimerQuery);
}

/// Entry point for the update and draw loops.
/// @param Time elapsed since previous call to update.
void Quantize::update(float dt) {

    const double startTime = GetTiming();

    // Generic variable used for statistical reporting.
    double time;

    // Frame counter
    stats.frames++;

    if(width >= 16384 || scene.empty()) {
        Exit("Too many or too few vertices in scene: %d/16384 bytes.", width);
    }

    camera.update(dt);

    glClearColor(0.541, 0.361, 0.361, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(_programRaytracer);
    glBindVertexArray(_vaoFrame);
    GLError();
    
    time = GetTiming();
    
    // Build Structure of arrays (SOA) from Array of structures (AOS)
    std::vector<Vector3> position;
    std::vector<Color> diffuse;
    std::vector<Color> specular;
    std::vector<Color> ambient;

    // Collect light properties
    for(const Light& light : lights) {
        position.push_back(light.position * 100);
        diffuse.push_back(light.diffuse);
        specular.push_back(light.specular);
        ambient.push_back(light.ambient);
    }

    // Amount of lights
    int nLights = (int) lights.size();
    
    
    // Upload the lights and other uniforms to the GPU
    glUniform1i(_lightCount, nLights);
    glUniform3fv(_lightsPosition, nLights, position[0].v);
    glUniform4fv(_lightsAmbiant, nLights, ambient[0].v);
    glUniform4fv(_lightsSpecular, nLights, specular[0].v);
    glUniform4fv(_lightsDiffuse, nLights, diffuse[0].v);
    glUniformMatrix4fv(_uniformRtRotation, 1, GL_FALSE, camera.rotation().f);
    glUniformMatrix4fv(_uniformRtTranslation, 1, GL_FALSE, camera.translation().f);
    glUniform2f(_uniformRtWindowSize, width, height);
    GLError();
    
    stats.uniforms += GetTiming() - time;
    
    // Bind the triangle textures (up to 15)
    std::vector<int> textureSamplers;
    textureSamplers.reserve(Textures::samplers.size());
    for(int i = 0; i < Textures::samplers.size(); ++i) {
        // Texture enabling
        glActiveTexture(GL_TEXTURE1 + i);                       // Use texture n
        glBindTexture(GL_TEXTURE_2D, Textures::samplers[i]);    // Bind handle to n
        GLError();
        
        textureSamplers.push_back(i + 1);
    }
    
    // Inform the shader which sampler indices to use
    glUniform1iv(_uniformTextures, (int) textureSamplers.size(), & textureSamplers[0]);
    GLError();
    
    // Amount of triangles
    glUniform1i(_uniformNumTriangles, (int) scene.size() / 3);
    GLError();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    GLError();
    
    time = GetTiming();
    
    // Upload the whole scene as a texture. We should measure the performance
    // of this. Static objects could be uploaded just once.
    glTexImage2D(GL_TEXTURE_2D,                     // What (target)
             0,                                     // Mip-map level
             GL_RGB32F,                             // Internal format
             (GLint) scene.size() * 4,                                 // Width
             1,                                     // Height
             0,                                     // Border
             GL_RGB,                                // Format (how to use)
             GL_FLOAT,                              // Type   (how to intepret)
             scene[0].position.v                    // Data
    );
    GLError();

    stats.uploadingDataTexture += GetTiming() - time;
    
    // Set uniform sampler to use the data texture
    glUniform1i(_uniformDataTexture, 0);
    GLError();


    glEnableVertexAttribArray(_attrRtPosition);
    
    // Validate just before drawing. If there are errors, this will show them. The
    // actual draw call does not contain debug information at all.
    //GLValidateProgram(_programRaytracer);


    // http://www.lighthouse3d.com/tutorials/opengl-short-tutorials/opengl-timer-query/
    
    glBeginQuery(GL_TIME_ELAPSED, _glTimerQuery);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GLError();
    
    glEndQuery(GL_TIME_ELAPSED);
    

    glDisableVertexAttribArray(_vboRtVertices);
    GLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    GLError();

    time = GetTiming();

    // Swap double buffer
    glSwapAPPLE();
    
    
    GLint duration;
    glGetQueryObjectiv(_glTimerQuery, GL_QUERY_RESULT, &duration);
    
    stats.drawing += duration * 0.0000000001;
    
    //printf("%d\n", duration);
    
    stats.swapping = GetTiming() - time;
    
    stats.total += GetTiming() - startTime;

    
    time = GetTiming();
    if(time - _lastLogTime > _logInterval) {
    
        Vector3 position    = camera.position;
        Vector3 orientation = camera.orientation;
    
        printf("------------\n"
        "Frames:             %.2f per second\n"
        "Update time:        %f seconds\n"
        "Lights and camera:  %f seconds (%lu lights in %lu bytes)\n"
        "Uploading vertices: %f seconds\n"
        "Shader time:        %f seconds\n"
        "Vertices:           %lu (%lu triangles in %lu bytes)\n"
        "Position:           %.2f %.2f %.2f\n"
        "Orientation:        %.2f %.2f %.2f\n",
        
        
            1.0 / ((time - _lastLogTime) / stats.frames),
            stats.total / stats.frames,
            stats.uniforms / stats.frames,
            lights.size(), sizeof(lights[0]) * lights.size(),
            stats.uploadingDataTexture / stats.frames,
            stats.drawing,
            scene.size(), scene.size() / 3, sizeof(scene[0]) * scene.size(),
            position.x, position.y, position.z,
            orientation.x, orientation.y, orientation.z
            );
    
        stats.reset();
        _lastLogTime = time;
    }
    
}
