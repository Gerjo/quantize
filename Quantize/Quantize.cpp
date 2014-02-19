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
    //, _uniformSampler_1(0)
    , _uniformSamplers{0}
    , camera(new Camera())
    , kernelLerp(0.7f)
    , kernelType(6)
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
    glDeleteRenderbuffers(1, &_renderBuffer);
    glDeleteTextures(1, &_fboTexture);
    glDeleteFramebuffers(1, &_fbo);
    
    glDeleteProgram(_programPost);
    glDeleteProgram(_programMesh);
    
    glDeleteBuffers(1, &_vboFboVertices);
}

void Quantize::loadDemoScene() {

    cube = Collada::FromFile("models/cube.dae");

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
    for(int i = 0, s = 6, n = 20; i < n; ++i) {
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
}


void Quantize::initialize(float width, float height) {
    if(_programMesh != 0) {
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
    
    // Usual perspective matrix
    _projection = Matrix44::CreatePerspective(
        3.14159268/2.5f,      // Field of view
        width/height,         // Aspect ratio
        0.5f,                 // near
        200.0f                // far
    );
    
    
    // Setup the mesh rendering shader programs
    initializeMeshProgram();
    
    // Setup the FBO, RBO and related shaders.
    initializePostProgram();
    
    loadDemoScene();
};

void Quantize::initializeMeshProgram() {
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
    _attrSamplerIndex = glGetAttribLocation(_programMesh, "samplerIndex");
    _uniformCamera    = glGetUniformLocation(_programMesh, "camera");
    _uniformModelTransform  = glGetUniformLocation(_programMesh, "modelTransform");
    _uniformNormalTransform = glGetUniformLocation(_programMesh, "normalTransform");
    //_uniformSampler_1 = glGetUniformLocation(_programMesh, "sampler_1");
    GLError();

    for(int i = 0; i < 15; ++i) {
        const string address = "samplers[" + std::to_string(i) + "]";
        _uniformSamplers[i] = glGetUniformLocation(_programMesh, address.c_str());
        
        printf("%s = %i\n", address.c_str(), _uniformSamplers[i]);

    }

    _lightCount     = glGetUniformLocation(_programMesh, "lightCount");
    _lightsPosition = glGetUniformLocation(_programMesh, "lightsPosition");
    _lightsDiffuse  = glGetUniformLocation(_programMesh, "lightsDiffuse");
    _lightsSpecular = glGetUniformLocation(_programMesh, "lightsSpecular");
    _lightsAmbiant  = glGetUniformLocation(_programMesh, "lightsAmbiant");
    GLError();

    // Remove the shaders, they are compiled and no longer required.
    glDetachShader(_programMesh, vsh);
    glDeleteShader(vsh);
    glDetachShader(_programMesh, fsh);
    glDeleteShader(fsh);
}

void Quantize::initializePostProgram() {
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
    
    _programPost = glCreateProgram();
    glAttachShader(_programPost, postvsh);
    glAttachShader(_programPost, postfsh);
    glLinkProgram(_programPost);
    glGetProgramiv(_programPost, GL_LINK_STATUS, &link_ok);
    
    if ( ! link_ok) {
        Exit("Linking failed.");
    }
    
    GLValidateProgram(_programPost);

    // Get a handle to the variables in the shader programs
    _attrUvFBO         = glGetAttribLocation(_programPost, "position");
    _uniformFboTexture = glGetUniformLocation(_programPost, "texture");
    _uniformWindowSize = glGetUniformLocation(_programPost, "windowSize");
    
    _uniformKernelType =  glGetUniformLocation(_programPost, "kernelType");
    _uniformKernelLerp =  glGetUniformLocation(_programPost, "kernelLerp");
    
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
void Quantize::render(Model& model, const Matrix44& transform) {
    
    Matrix44 t = model.transform * transform;
    
    /*if(false && model.texture != 0) {
        // Texture enabling
        glActiveTexture(GL_TEXTURE0);                    // Use texture 0
        glBindTexture(GL_TEXTURE_2D, *(model.texture.get()));    // Work with this texture
        glUniform1i(_uniformSampler_1, 0);               // Set the sampler to tex 0
        GLError();
    }*/
    
    // Set the perspective projection, once. This is shared among all models.
    glUniformMatrix4fv(_uniformModelTransform,  // Location
                        1,                      // Amount of matrices
                        false,                  // Require transpose
                        t.f                     // Float array with values
    );
    GLError();
    
    // Set the perspective projection, once. This is shared among all models.
    glUniformMatrix3fv(_uniformNormalTransform,  // Location
                        1,                       // Amount of matrices
                        false,                   // Require transpose
                        t.GetMatrix33().Invert().f        // Float array with values
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
    
    
    
    glVertexAttribPointer(_attrSamplerIndex,                  // The attribute in the shader.
                        1,                                    // Number of "fields", in this case 1: the index.
                        GL_UNSIGNED_INT,                      // Data type
                        GL_FALSE,                             // Must these values be normalized? No thanks.
                        sizeof(VertexData),                   // Size of each structure
                        (void*) offsetof(VertexData, sampler) // Offset
    );
    GLError();
    
    glEnableVertexAttribArray(_attrSamplerIndex);
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
void Quantize::update(float dt) {

    // Enable framebuffer render target
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    GLFBError();
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Camera position (insert FPS code here).
    camera->update();
    Matrix44 transform = camera->transform();
    
    
    
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


    std::vector<Vector3> position;
    std::vector<Color> diffuse;
    std::vector<Color> specular;
    std::vector<Color> ambient;

    for(const Light& light : lights) {
        position.push_back(light.position);
        diffuse.push_back(light.diffuse);
        specular.push_back(light.specular);
        ambient.push_back(light.ambient);
    }

    // Amount of lights
    int nLights = (int) lights.size();
    
    glUniform1i(_lightCount, nLights);
    GLError();
    
    glUniform3fv(_lightsPosition, nLights, position[0].v);
    glUniform4fv(_lightsAmbiant, nLights, ambient[0].v);
    glUniform4fv(_lightsSpecular, nLights, specular[0].v);
    glUniform4fv(_lightsDiffuse, nLights, diffuse[0].v);
    GLError();


    for(int i = 0, c = Textures::samplers.size(); i < c; ++i) {
        // Texture enabling
        glActiveTexture(GL_TEXTURE0 + i);                       // Use texture n
        glBindTexture(GL_TEXTURE_2D, Textures::samplers[i]);    // Bind handle to n
        glUniform1i(_uniformSamplers[i], i);                    // Set the sampler to tex n
        GLError();
    }

    // Render/Update loop
    for(auto model : entities) {
        model->update(this, Matrix44::CreateIdentity(), dt);
    }


    // Visualise the physical location of lights
    for(Light& light : lights) {
        Matrix44 transform = Matrix44::CreateTranslation(light.position.x, light.position.y, light.position.z)
        * Matrix44::CreateScale(0.3f);
        
        cube->update(this, transform, dt);
    }

    // Stop rendering to buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLFBError();


    glClearColor(0.541, 0.361, 0.361, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(_programPost);
    
    glUniform1f(_uniformKernelLerp, kernelLerp);
    glUniform1i(_uniformKernelType, kernelType);
    
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
