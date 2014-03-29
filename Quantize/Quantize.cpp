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
    
    /*float f[] = {
        30.736634111963212,34.848437015898526,87.97128084115684,
        61.9190025376156,26.234959112480283,31.78147403523326,
        88.39382929727435,95.20727114286274,35.9192576026544,
        70.72803929913789,60.23380230180919,84.58086540922523,
        6.402283837087452,80.45414371881634,67.16174038592726,
        21.880165627226233,12.75157816708088,9.1642199549824,
        23.191527533344924,50.79917625989765,62.02671288046986,
        83.27980369795114,93.60271936748177,66.71366228256375,
        73.98815324995667,36.01606253068894,96.17416446562856,
        77.16044730041176,95.45318209566176,84.80310190934688};
    std::deque<Photon> photons;
    
    for(int i = 0; i < 10 * 3; i += 3) {
        Photon photon;
        photon.position.x = f[i + 0];
        photon.position.y = f[i + 1];
        photon.position.z = f[i + 2];
        
        photons.push_back(photon);
    }
    
    KdTree tree(photons);
    
    auto array = tree.toVector();
    
    int i = 0;
    for(auto v : array) {
        printf("%d %.6f, %.6f, %.6f\n", i++, v.position.x, v.position.y, v.position.z);
    }
    
    exit(0);*/

    Light light;
    light.position.x = 0;//15.0f;
    light.position.y = 5.0f;
    light.position.z = 0;//20.0f;
    
    for(size_t i = 0; i < 4; ++i) {
        light.ambient.v[i]  = 0.2f;
        light.diffuse.v[i]  = 0.2f;
        light.specular.v[i] = 0.0f;
    }
  
    light.diffuse.r = 1.0;
    light.diffuse.g = 1.0;
    light.diffuse.b = 1.0;
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
    glDeleteVertexArrays(1, &_vaoFrame);
    glDeleteBuffers(1, &_vboRtVertices);
    // TODO: double check this list. I must've missed many gl items.
}

void Quantize::loadDemoScene() {
    //Textures::LoadPNG("models/tmp/red.png");
    
    cube      = Collada::FromFile("models/Magey/Magey.dae");
    rectangle = Collada::FromFile("models/Plane/checkerboard3.dae");
    triangle  = Collada::FromFile("models/Plane/triangle.dae");
    
    model     = Collada::FromFile("models/Rock1/Rock1.dae");
    
    //entities.push_back(Collada::FromFile("models/cube.dae"));

    // Custom designed cube by Master Mergon with textures forged from the
    // lava of Mount Doom enriched with tiny ring-like particles.
    //scene.insert(scene.end(), ((Model*)cube->sub[0].get())->vertices.begin(), ((Model*)cube->sub[0].get())->vertices.end());
    
    // A checkerboard of checkerboards.
    /*float scale = 25;
    for(int x = 0; x < 1; ++x) {
        for(int y = 0; y < 1; ++y) {
            Matrix44 t = Matrix44::CreateRotateX(3.14 / 2) * Matrix44::CreateTranslation(x * scale * 2, y * scale * 2, 0) * Matrix44::CreateScale(scale);
            
            for(VertexData d : ((Model*)rectangle->sub[0].get())->vertices) {
                d.position = t * d.position;
            
                scene.push_back(d);
            }
        }
    }*/
    
    auto &vertices = ((Model*)cube->sub[0].get())->vertices;
    
    // Mega cube
    for(VertexData d : vertices) {
        d.position = Matrix44::CreateTranslation(0, 0, 0) * Matrix44::CreateScale(Vector3(20, 10, 20)) * d.position;
        scene.push_back(d);
    }
    
    // Mini cube
    for(VertexData d : vertices) {
        d.position = Matrix44::CreateTranslation(0, 0, 0) * Matrix44::CreateScale(Vector3(1, 1, 1)) * d.position;
        scene.push_back(d);
    }
    
    // Tile
    for(VertexData d : ((Model*)rectangle->sub[0].get())->vertices) {
        d.position = Matrix44::CreateTranslation(0, 0, 0)
        * Matrix44::CreateScale(Vector3(4, 4, 4))
        * Matrix44::CreateRotate(1.23, 1, 1, 1)
        * d.position;
        scene.push_back(d);
    }
    
    
    
    assert(scene.size() % 3 == 0);
    
    for(size_t i = 0; i < scene.size(); i += 3) {
    
        Face face;
        
        face.a = scene[i + 0].position;
        face.b = scene[i + 1].position;
        face.c = scene[i + 2].position;
        
        
        face.u = scene[i + 0].uv;
        face.v = scene[i + 1].uv;
        face.w = scene[i + 2].uv;
        
        face.sampler = scene[i].sampler;
    
        faces.push_back(face);
    }

    /*
    printf("Will insert: %lu triangles.\n", vertices.size()/3);
    
    Tree tree;

    
    for(size_t i = 0; i < vertices.size(); i += 3) {
        tree.insert(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    
    tree.print();
    
    printf("Tree bytes: %lu, root bytes: %lu\n", tree.size(), tree.root.triangles.size() * sizeof(VertexData) * 3);
    */
    
    
    shootPhotons();
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
    glDisable(GL_DEPTH_TEST);
    
    // Hide faces not facing us.
    glDisable(GL_CULL_FACE);
    
    //glDepthFunc(GL_ALWAYS);
    
    // Premultiplied alpha. (I think? I always confuse the two)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
      
    // Affine coordinate transformation. We can do some scaling here,
    // e.g., map [-1,1] to [0, screensize] - though it makes more sense
    // to use a matrix (_projection) for that.
    glViewport(0, 0, width, height);
        
    // Experimental raytacer
    initializeRaytraceProgram();
    
    initializePhotonProgram();

    loadDemoScene();
};


/// Useful links: http://ephenationopengl.blogspot.nl/2012/01/setting-up-deferred-shader.html
///
void Quantize::initializePhotonProgram() {

    // Texture to hold the photon data
    glGenTextures(1, &photon.photonTexture);
    glBindTexture(GL_TEXTURE_2D, photon.photonTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLError();

    // We render to a texture, so let's create a texture.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(3, photon.texture);
    
    for(size_t i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, photon.texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, photon.width, photon.height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLError();
    }
    
    // Depth buffer  (Render Buffer Object)
    glGenRenderbuffers(1, &photon.renderBuffer);
    glGenFramebuffers(1, &photon.fbo);
    GLError();
    
    glBindRenderbuffer(GL_RENDERBUFFER, photon.renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, photon.width, photon.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    GLError();
    
    glBindFramebuffer(GL_FRAMEBUFFER, photon.fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, photon.renderBuffer);
    GLError();
    
    for(int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, photon.texture[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, photon.texture[i], 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLError();
    }
    
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        Exit("glCheckFramebufferStatus: error %p.", status);
    }
  
    
    // Disable buffer, for now.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLError();
    
    // Prepare all shaders. These will exit on failure.
    GLuint vsh = CompileShader("shaders/photon.vsh");
    GLuint fsh = CompileShader("shaders/photon.fsh");
    
    photon.program = glCreateProgram();
    glAttachShader(photon.program, vsh);
    glAttachShader(photon.program, fsh);
    GLError();
    
    glBindFragDataLocation(photon.program, 0, "outColor");
    glBindFragDataLocation(photon.program, 1, "outPosition");
    glBindFragDataLocation(photon.program, 2, "outMeta");
    GLError();
    
    glLinkProgram(photon.program);
    GLError();

    photon.attrPosition = glGetAttribLocation(photon.program, "position");
    photon.uniformWindowSize = glGetUniformLocation(photon.program, "windowSize");
    photon.uniformLightCount = glGetUniformLocation(photon.program, "lightCount");
    photon.uniformLightsPosition = glGetUniformLocation(photon.program, "lightPositions");
    photon.uniformData = glGetUniformLocation(photon.program, "zdata");
    photon.unformTriangleCount = glGetUniformLocation(photon.program, "triangleCount");
    photon.uniformTextures     = glGetUniformLocation(_programRaytracer, "textures");

    GLError();
    
    // The rectangle used to render onto, the UVs are derived from this.
    GLfloat fbo_vertices[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };
    
    glGenVertexArrays(1, &photon.vao);
    glBindVertexArray(photon.vao);
    
    glGenBuffers(1, &photon.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, photon.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    GLError();
    
    glVertexAttribPointer(
            photon.attrPosition,         // attribute
            2,                           // number of elements per vertex, here (x,y)
            GL_FLOAT,                    // the type of each element
            GL_FALSE,                    // take our values as-is
            0,                           // no extra data between each position
            0                            // offset of first element
    );
    GLError();
    
    glEnableVertexAttribArray(photon.attrPosition);
    GLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLError();
    
    // Cleanup
    glDetachShader(photon.program, vsh);
    glDeleteShader(vsh);
    glDetachShader(photon.program, fsh);
    glDeleteShader(fsh);
    GLError();
    
    
}

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
    GLError();

    _uniformRtWindowSize  = glGetUniformLocation(_programRaytracer, "windowSize");
    _attrRtPosition       = glGetAttribLocation(_programRaytracer, "vertexPosition");
    _uniformNumTriangles  = glGetUniformLocation(_programRaytracer, "numTriangles");
    _uniformRtRotation    = glGetUniformLocation(_programRaytracer, "rotation");
    _uniformRtTranslation = glGetUniformLocation(_programRaytracer, "translation");
    _uniformPhotonTexture = glGetUniformLocation(_programRaytracer, "photons");
    _uniformNumPhotons    = glGetUniformLocation(_programRaytracer, "numPhotons");

    _uniformTextures     = glGetUniformLocation(_programRaytracer, "textures");
    _uniformDataTexture  = glGetUniformLocation(_programRaytracer, "zdata");
    _uniformTime         = glGetUniformLocation(_programRaytracer, "time");
    _uniformFrameCounter = glGetUniformLocation(_programRaytracer, "frameCounter");
    GLError();
    
    _uniformN          = glGetUniformLocation(_programRaytracer, "n");
    _uniformSigma      = glGetUniformLocation(_programRaytracer, "sigma");
    _uniformRange      = glGetUniformLocation(_programRaytracer, "range");
    _uniformJitter     = glGetUniformLocation(_programRaytracer, "enableJitter");
    _uniformUseTexture = glGetUniformLocation(_programRaytracer, "useTexture");
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
    ++_frameCounter;
    
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
    glEnable(GL_BLEND);
    
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
        position.push_back(light.position);
        diffuse.push_back(light.diffuse);
        specular.push_back(light.specular);
        ambient.push_back(light.ambient);
    }

    // Amount of lights, when disabled - simply upload nothing.
    const int nLights = std::min(enableLights, (int) lights.size());
    
    
    // Upload the lights and other uniforms to the GPU
    glUniform1i(_lightCount, nLights);
    //glUniform1i(_lightCount, 0);
    glUniform3fv(_lightsPosition, nLights, position[0].v);
    glUniform4fv(_lightsAmbiant, nLights, ambient[0].v);
    glUniform4fv(_lightsSpecular, nLights, specular[0].v);
    glUniform4fv(_lightsDiffuse, nLights, diffuse[0].v);
    
    glUniformMatrix4fv(_uniformRtRotation, 1, GL_FALSE, camera.rotation().f);
    glUniformMatrix4fv(_uniformRtTranslation, 1, GL_FALSE, camera.translation().f);
    glUniform2f(_uniformRtWindowSize, width, height);
    GLError();
    
    // Upload raytracing properties
    glUniform1i(_uniformN, n);
    glUniform1f(_uniformSigma, sigma);
    glUniform1f(_uniformRange, range);
    glUniform1i(_uniformJitter, (int) enableJitter);
    glUniform1i(_uniformUseTexture, (int) useTexture);
    GLError();
    
    
    
    glUniform1i(_uniformFrameCounter, (int) _frameCounter);
    glUniform1f(_uniformTime, (float) GetTiming());
    GLError();
    stats.uniforms += GetTiming() - time;
    
    // Bind the triangle textures (up to 15)
    std::vector<int> textureSamplers;
    textureSamplers.reserve(Textures::samplers.size());
    for(int i = 0; i < Textures::samplers.size(); ++i) {
        // Texture enabling
        glActiveTexture(GL_TEXTURE2 + i);                       // Use texture n
        glBindTexture(GL_TEXTURE_2D, Textures::samplers[i]);    // Bind handle to n
        GLError();
        
        textureSamplers.push_back(i + 2);
    }
    
    // Inform the shader which sampler indices to use
    glUniform1iv(_uniformTextures, (int) textureSamplers.size(), & textureSamplers[0]);
    GLError();
    
    // Amount of triangles
    glUniform1i(_uniformNumTriangles, (int) faces.size());
    GLError();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    GLError();
    
    time = GetTiming();
    
    glTexImage2D(GL_TEXTURE_2D,                     // What (target)
             0,                                     // Mip-map level
             GL_RGB32F,                             // Internal format
             (GLint) faces.size() *
             (sizeof(Face) / 3),                    // Width
             1,                                     // Height
             0,                                     // Border
             GL_RGB,                                // Format (how to use)
             GL_FLOAT,                              // Type   (how to intepret)
             faces[0].a.v                           // Data
    );
    GLError();

    stats.uploadingDataTexture += GetTiming() - time;
    
    // Set uniform sampler to use the data texture
    glUniform1i(_uniformDataTexture, 0);
    GLError();

    // Enable photon map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, photon.photonTexture);
    glUniform1i(_uniformPhotonTexture, 1);
    glUniform1i(_uniformNumPhotons, (int) kdtree.size());
    GLError();

    glEnableVertexAttribArray(_attrRtPosition);
    
    // Validate just before drawing. If there are errors, this will show them. The
    // actual draw call does not contain debug information at all.
    GLValidateProgram(_programRaytracer);


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
    
    GLint duration;
    glGetQueryObjectiv(_glTimerQuery, GL_QUERY_RESULT, &duration);
    
    stats.drawing += duration * 0.0000000001;
    
    
    
    stats.total += GetTiming() - startTime;

    handleLogging();
    

}

////////////////////////////////////////////////////////////////////////////////
// Photon stuff
////////////////////////////////////////////////////////////////////////////////
void Quantize::shootPhotons() {

    // Enable framebuffer render target
    glBindFramebuffer(GL_FRAMEBUFFER, photon.fbo);
    GLFBError();
    glClearColor(0, 0, 0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    
    // Photon program enable!
    glUseProgram(photon.program);
    
    // Draw into the following buffers (Color, position and meta)
    static GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, buffers);
    GLError();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dataTexture);
    
    printf("Uploading scene...");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, (GLint) faces.size() * (sizeof(Face) / 3),  1, 0, GL_RGB, GL_FLOAT, faces[0].a.v);
    GLError();
    printf(" done.\n");

    // Amount of lights, when disabled - simply upload nothing.
    const int nLights = std::min(enableLights, (int) lights.size());
    
    // Build Structure of arrays (SOA) from Array of structures (AOS)
    std::vector<Vector3> position;
    
    // Collect light properties
    for(const Light& light : lights) {
        position.push_back(light.position);
    }
    
    glUniform1i(photon.uniformLightCount, nLights);
    glUniform3fv(photon.uniformLightsPosition, nLights, position[0].v);
    glUniform1i(photon.unformTriangleCount, (int) faces.size());
    glUniform1i(photon.uniformData, 0);
    glUniform2f(photon.uniformWindowSize, width, height);
    GLError();
    
      
    // Bind the triangle textures (up to 15)
    std::vector<int> textureSamplers;
    textureSamplers.reserve(Textures::samplers.size());
    for(int i = 0; i < Textures::samplers.size(); ++i) {
        // Texture enabling
        glActiveTexture(GL_TEXTURE2 + i);                       // Use texture n
        glBindTexture(GL_TEXTURE_2D, Textures::samplers[i]);    // Bind handle to n
        GLError();
        
        textureSamplers.push_back(i + 2);
    }
    
    // Inform the shader which sampler indices to use
    glUniform1iv(photon.uniformTextures, (int) textureSamplers.size(), & textureSamplers[0]);
    GLError();

    
    glBindVertexArray(photon.vao);
    GLError();
    
    printf("Issuing command to shoot %d photons.\n", photon.width * photon.height);
    
    
    GLValidateProgram(photon.program);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLError();

    // Stop rendering to the buffer. Enable rendering to screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLFBError();
    GLError();
    
    //static GLfloat* pixels = new GLfloat[(int)width * (int)height * 4];
    
    // The GPU returns RGBA. We simply discard the "A" component.
    int channels = 4;
    
    // Allocate some memory to hold the to be retrieve data.
    std::vector<float> colors((int)photon.width * (int)photon.height * channels, 0);      // 1
    std::vector<float> positions((int)photon.width * (int)photon.height * channels, 0);   // 2
    std::vector<float> meta((int)photon.width * (int)photon.height * channels, 0);        // 3
    
    glFinish();
    
    // Retrieve the photon colors from the GPU
    glBindTexture(GL_TEXTURE_2D, photon.texture[0]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, & colors[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLError();
    
    // Retrieve the photon positions from the GPU
    glBindTexture(GL_TEXTURE_2D, photon.texture[1]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, & positions[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLError();
    
    // Retrieve the photon meta data from the GPU
    glBindTexture(GL_TEXTURE_2D, photon.texture[2]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, & meta[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLError();
    
    
    std::deque<Photon> photons;
    
    int discarded = 0;
    
    // From the arrays, create photon structs.
    for(int i = 0; i < photon.width * photon.height * channels; i += channels) {
        
        // Discard "null" photons, those that hit nothing.
        if(positions[i] == 0 && positions[i + 1] == 0 && positions[i + 2] == 0) {
            ++discarded;
        } else {
            Photon photon(&positions[i], &colors[i], &meta[i]);
        
            photons.push_back(photon);
        }
    }
    
    printf("Discarded %d dead photons :(\n", discarded);
    
    printf("Building KdTreee...");
    
    // Make the KdTree!
    KdTree tree(photons);
    
    printf(" done.\n");

    printf("Exporting %lu photons to vector... ", photons.size());
    kdtree = tree.toVector();
    
    //kdtree[0].color(1, 1, 1);
    //kdtree[0].position(1, 1, 1);
    //kdtree[0].meta(1, 1, 1);
    
    printf(" %lu items in tree. Done.\n", kdtree.size());
    
    
    printf("Uploading kdtree to GPU...");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, photon.photonTexture);
    
    if(kdtree.size() > 16000) {
        Exit("Too many. Wrap around texture rows.");
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, (int) kdtree.size() * 3, 1, 0, GL_RGB, GL_FLOAT, kdtree[0].color.v);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLError();

    printf(" done.\n");
    
    
    for(int i = 0; i < kdtree.size(); ++i) {
        if( ! isinf(kdtree[i].position.x)) {
            printf("Photon #%d [%.5f %.5f %.5f]\n", i, kdtree[i].position.x, kdtree[i].position.y, kdtree[i].position.z);
        }
    }
}

void Quantize::handleLogging() {
    const double time = GetTiming();
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
            "Orientation:        %.2f %.2f %.2f\n"
            "Photons:            %lu\n",
        
            1.0 / ((time - _lastLogTime) / stats.frames),
            stats.total / stats.frames,
            stats.uniforms / stats.frames,
            lights.size(), sizeof(lights[0]) * lights.size(),
            stats.uploadingDataTexture / stats.frames,
            stats.drawing,
            faces.size() * 3, faces.size(), sizeof(faces[0]) * faces.size(),
            position.x, position.y, position.z,
            orientation.x, orientation.y, orientation.z,
            kdtree.size()
        );
    
        stats.reset();
        _lastLogTime = time;// + 10000000;
    }

}
