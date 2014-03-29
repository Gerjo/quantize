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

// 6 float triplets
struct Face {
    Vector3 a;
    Vector3 b;
    Vector3 c;
    
    Vector2 u;
    GLfloat sampler{1};
    
    Vector2 v;
    GLfloat padding1{0};
    
    Vector2 w;
    GLfloat padding2{0};
    
    // Normals
    Vector3 n1;
    Vector3 n2;
    Vector3 n3;
};

/// 4 float triplets
struct VertexData {

    /// Position in the world
    Vector3 position;
    
    /// Normal per vertex
    Vector3 normal;
    
    /// UV coordinates with UV repeat enabled.
    Vector2 uv;
    
    /// RGBA blend color per vertex. Not used atm.
    unsigned char color[4] = {2, 255, 2, 2};
    
    //int color;
    
    /// Texture sampler index.
    GLfloat sampler = {1};
    
    // Bring storage alignment to (bytes mod 3 == 0)
    GLfloat padding[2] = {0};
    
    VertexData(Vector3 position,  Vector3 normal = Vector3(0, 0, 0), Vector2 uv = Vector2(1, 1))
        : position(position)
        , normal(normal)
        , uv(uv)
        {
            // codes
    }
    
    VertexData(float x=0,float y=0,float z=0,float a=0,float b=0,float c=0, float u = 1, float v = 1)
        : position(x, y, z)
        , normal(a, b, c)
        , uv(u, v) {
            // codes
    }};

struct Model : public Entity {

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
};