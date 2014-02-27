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

Quantize::Quantize()
    : width(200)
    , height(100)
    , camera(new Camera())
    {
    
    Light light;
    light.position.x = 15.0f;
    light.position.y = 5.0f;
    light.position.z = -20.0f;
    
    for(size_t i = 0; i < 4; ++i) {
        light.ambient.v[i]  = 0.2f;
        light.diffuse.v[i]  = 0.2f;
        light.specular.v[i] = 0.0f;
    }
  
    light.diffuse.r = 0;
    light.diffuse.g = 0;
    light.diffuse.b = 2;
    lights.push_back(light);
  
        
    light.diffuse.r = 1;
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
    lights.push_back(light);
}

Quantize::~Quantize() {
    glDeleteProgram(_programRaytracer);
}

void Quantize::loadDemoScene() {
    //Textures::LoadPNG("models/tmp/red.png");
    
    cube      = Collada::FromFile("models/Magey/Magey.dae");
    rectangle = Collada::FromFile("models/Plane/checkerboard2.dae");
    triangle  = Collada::FromFile("models/Plane/triangle.dae");
    
    entities.push_back(Collada::FromFile("models/cube.dae"));

    //return;

    /*for(int i = 0; i < 4; ++i) {
        Entity* e = new Entity();
    

        if(i == 0)
            e->sub.push_back(Collada::FromFile("models/P39 AIRACOBRA/p39.dae"));
        
        if(i == 1)
            e->sub.push_back(Collada::FromFile("models/T-90/T-90.dae"));
        
        if(i == 2)
            e->sub.push_back(Collada::FromFile("models/FW 190/FW190.dae"));
        
        if(i == 3)
            e->sub.push_back(Collada::FromFile("models/FV510 Warrior/fv510.dae"));
        
        //e->sub.push_back(Collada::FromFile("models/A10/A10.dae"));
        e->transform.SetTranslation(1000 * i, 0, 0);


        entities.push_back(std::shared_ptr<Entity>(e));
    }*/
    
    /*for(int i = 0; i < 4; ++i) {
        Entity* e = new Entity();
    
        e->sub.push_back(Collada::FromFile("models/cube.dae"));
        e->transform.SetTranslation(1000 * i, 0, 0);

        entities.push_back(std::shared_ptr<Entity>(e));
    }
    */
    for(int i = 0, s = 9, n = 20; i < n; ++i) {
        for(int j = 0; j < n; ++j) {

            Entity* e = new Entity();
        
            e->sub.push_back(Collada::FromFile("models/Plane/plane.dae"));

            e->transform.SetTranslation(s * (n*0.5 - i), 0, s * (n*0.5 - j));
            
            entities.push_back(std::shared_ptr<Entity>(e));
        }
    }
    
    for(int i = 0; i < 4; ++i) {
        Entity* e = new Entity();
    
        e->sub.push_back(Collada::FromFile("models/AUSFB/ausfb.dae"));
        e->transform.SetTranslation(1000 * i, 0, 3000);
        
        entities.push_back(std::shared_ptr<Entity>(e));
    }
    
    //entities.push_back(std::shared_ptr<Entity>(Collada::FromFile("models/Plane/plane.dae")));
    
    int i = 0;
    for(auto pair : Textures::cache) {
        printf("[sampler: %d] texture: %s\n", Textures::samplers[i], pair.first.c_str());
        
        ++i;
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

    // Data texture to hold vertices gerjo
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_dataTexture);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLError();
}

/// Entry point for the update and draw loops.
/// @param Time elapsed since previous call to update.
void Quantize::update(float dt) {
    camera->update();

    glClearColor(0.541, 0.361, 0.361, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(_programRaytracer);
    GLError();
    
    glUniform2f(_uniformRtWindowSize, width, height);
    GLError();
    
    
    auto subject = cube;
    
   
    glUniformMatrix4fv(_uniformRtRotation, 1, GL_FALSE, camera->rotation().f);
    glUniformMatrix4fv(_uniformRtTranslation, 1, GL_FALSE, camera->translation().f);
    
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
    
    // Some group of faces to upload
    std::vector<VertexData>& vertices = ((Model*)subject->sub[0].get())->vertices;
   
    // Amount of triangles
    glUniform1i(_uniformNumTriangles, (int) vertices.size() / 3);
    GLError();
    
    GLfloat* data = vertices[0].position.v;
    
    // 4 triplets with floats
    const GLint width  = (GLint) vertices.size() * 4;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    GLError();
    
    glTexImage2D(GL_TEXTURE_2D,                     // What (target)
             0,                                     // Mip-map level
             GL_RGB32F,                             // Internal format
             width,                                 // Width
             1,                                     // Height
             0,                                     // Border
             GL_RGB,                                // Format (how to use)
             GL_FLOAT,                              // Type   (how to intepret)
             data);                                 // Data
    GLError();

    
    // Set uniform sampler to use the data texture
    glUniform1i(_uniformDataTexture, 0);
    GLError();
    
    // From this point onward we render a rectangle, this rectangle serves as a
    // raytrace canvas.
    glEnableVertexAttribArray(_attrRtPosition);
    GLError();
    
    glBindVertexArray(_vaoFrame);
    
    GLValidateProgram(_programRaytracer);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GLError();
    
    glDisableVertexAttribArray(_vboRtVertices);
    GLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLError();

    // Run draw calls.
    //glFlush();
    glSwapAPPLE();
}
