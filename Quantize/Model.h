//
//  Model.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Tools.h"
#include <vector>
#include <memory>
#include "Math/Vector3.h"
#include "Math/Matrix44.h"
#include "Math/Matrix33.h"
#include "Entity.h"

class Quantize;

using namespace Furiosity;

struct VertexData {

    /// Position in the world
    Vector3 position;
    
    /// Normal per vertex
    Vector3 normal;
    
    /// UV coordinates with UV repeat enabled.
    Vector2 uv;
    
    /// RGBA blend color per vertex. Not used atm.
    unsigned char color[4] = {(unsigned char)0, 255, 0, 0};
    
    /// Texture sampler index.
    GLuint sampler;
    
    VertexData(Vector3 position,  Vector3 normal = Vector3(0, 0, 0), Vector2 uv = Vector2(1, 1))
        : position(position)
        , normal(normal)
        , uv(uv)
        , sampler(198707891) {
            // codes
    }
    
    VertexData(float x=0,float y=0,float z=0,float a=0,float b=0,float c=0, float u = 1, float v = 1)
        : position(x, y, z)
        , normal(a, b, c)
        , uv(u, v) {
            // codes
    }};

class Model : public Entity {
private:
    /// Flag to indicate if this texture has been uploaded already, i.e. are the
    /// vbo's ready.
    bool _isuploaded;

public:
    /// Pointer to vertex buffer objects.
    ///  0: the vertex data
    ///  1: the order of drawing indices
    GLuint vbo[2];
    
    GLuint vao;
    
    /// The vertexdata (as per vbo[0]
    std::vector<VertexData> vertices;
    
    /// The drawing indices (as per vbo[1]
    std::vector<unsigned int> indices;

    /// Identifiers as specified in the obj file. These are here for legacy
    /// support of the old model loader.
    std::string material;
    std::string group;
    
    // Some name debug associated with this model. Generally taken from
    // the Collada file.
    std::string name;

    /// A handle to the texture. Smart pointers are used to manage the resource
    /// reuse.
    std::shared_ptr<GLuint> texture;
 
    /// Construct a
    Model();
    ~Model();
    
    /// Determine if the vbo's are uplaoded.
    bool isUpoaded();
    virtual void update(Quantize* q, const Matrix44& parent, const float dt) override;
    
    void upload(Quantize* quantize);
};