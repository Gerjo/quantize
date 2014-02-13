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

using namespace Furiosity;

struct VertexData {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
    unsigned char color[4] = {(unsigned char)0, 255, 0, 0};
    
    VertexData(Vector3 position,  Vector3 normal = Vector3(0, 0, 0), Vector2 uv = Vector2(1, 1))
        : position(position)
        , normal(normal)
        , uv(uv) {
            // codes
    }
    
    VertexData(float x=0,float y=0,float z=0,float a=0,float b=0,float c=0, float u = 1, float v = 1)
        : position(x, y, z)
        , normal(a, b, c)
        , uv(u, v) {
            // codes
    }};

class Model {
private:
    /// Flag to indicate if this texture has been uploaded already, i.e. are the
    /// vbo's ready.
    bool _isuploaded;
    
    /// Will be removed. soon.
    float angle;
    
public:
    /// Pointer to vertex buffer objects.
    ///  0: the vertex data
    ///  1: the order of drawing indices
    GLuint vbo[2];
    
    /// Transform as specified in the collada file. This is shared among all
    /// meshes. Generally this normalizes the mesh somewhat.
    Matrix44 baseTransform;
    
    /// Transformation of this model.
    Matrix44 modelTransform;
    
    /// Transform for the normals. These are direction only, so require no
    /// translation.
    Matrix33 normalTransform;
    
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
    
    /// Models nested within this model. Transforms are inherited, this is used
    /// for Collada's nested nodes.
    std::vector<Model*> submodels;

    Model() : _isuploaded(false), angle(0), vbo{0}, texture(0) {
        modelTransform.SetIndentity();
        normalTransform.SetIdentity();
        baseTransform.SetIndentity();
    }
    
    bool isUpoaded() {
        return _isuploaded;
    }
    
    void update(const float dt) {
        angle += 0.006;
        
        //
        modelTransform  = Matrix44::CreateRotate(angle, 0, 1, 0) * baseTransform;
        
        normalTransform = modelTransform.GetMatrix33();
        
        modelTransform = Matrix44::CreateScale(0.03) * modelTransform;
    }
    
    void upload() {
        glGenBuffers(2, &vbo[0]);
        GLError();
        
        // Activate
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        // Activate
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        GLError();
        
        // Copy into VBO:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        GLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind buffer
        GLError();
        
        _isuploaded = true;
    }
};