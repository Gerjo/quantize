//
//  Parser.h
//  Quantize
//
//  Created by Gerard Meier on 05/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Tools.h"
#include "Model.h"

#include "Math/Vector3.h"
#include "Math/Vector2.h"

#include <vector>
#include <deque>
#include <sstream>
#include <cstdlib> // for atof

using std::string;

struct Parser {
    static std::vector<Model*> FromFile(const std::string& filename) {
        std::vector<Model*> models;
    
        if( ! FileExists(filename)) {
            Exit("File does not exist: %s.", filename.c_str());
        }
        
        std::string data = ReadFile(filename);
        
        if(data.empty()) {
            Exit("File is empty: %s.", filename.c_str());
        }
    
        // Using a deque to avoid resizing.
        std::deque<Vector3> vertices;   vertices.push_back(Vector3(0, 0, 0));
        std::deque<Vector2> uvs;        uvs.push_back(Vector2(0, 0));
        std::deque<Vector3> normals;    normals.push_back(Vector3(0, 0, 0));
    
        Model* model = new Model();
        models.push_back(model);

    
        // Parse per line.
        for(size_t linenumber = 0, run = 1, readOffset = 0; run; ++linenumber) {
            auto pos = data.find_first_of('\n', readOffset);
            
            // Special case for the last time (file may not end with a
            // empty newline)
            if(pos == string::npos) {
                pos = data.length();
                run = false;
            }
            
            // Extract a copy of the line
            std::string line = data.substr(readOffset, pos-readOffset);
            
            // Split per space
            std::vector<string> chunks = StringExplode(line, " ");
            
            // Account for empty lines
            if( ! chunks.empty()) {
                const string& key = chunks[0];
                
                if(key == "g") {
                    //printf("Creating new model for: %s\n", chunks[1].c_str());
                    
                    // Encountered a new group, only create a new model if the
                    // current model is empty. There are cases were "usemtl"
                    // comes before the group specification.
                    if( ! model->vertices.empty()) {
                        model = new Model();
                        models.push_back(model);
                    }
                    
                    model->group = chunks[1];

                } else if(StringStartsWith(key, "#")) {
                    // Sourcecode comment.
                    
                } else if(key == "mtllib") {
                    // Material file to use.
                    //model->material = chunks[1];
                    
                } else if(key == "usemtl") {
                    printf("Using material: %s\n", chunks[1].c_str());
                    
                    model->material = chunks[1];

                    // is texture?
                    
                } else if(key == "vt") {
                    // Neglecting any optional 3rd component.
                    // Flipping the 2nd coordinate to match OpenGL's spec.
                    uvs.push_back(Vector2(::atof(chunks[1].c_str()), 1-::atof(chunks[2].c_str())));
                
                } else if(key == "vn") {
                    normals.push_back(Vector3(::atof(chunks[1].c_str()), ::atof(chunks[2].c_str()), ::atof(chunks[3].c_str())));
                
                } else if(key == "v") {
                    vertices.push_back(Vector3(::atof(chunks[1].c_str()), ::atof(chunks[2].c_str()), ::atof(chunks[3].c_str())));
               
                } else if(key == "f") {
                
                    for(size_t i = 1; i < chunks.size(); ++i) {
                        auto nums = StringExplode(chunks[i], "/");
                        
                        // Some files have loads of empty spaces. Ignore those.
                        if( ! nums.empty()) {
                        
                            if(nums.size() < 2) {
                                Exit("Not enough face indices. We expect vertex and uv-mapping.\nFound: %s.\n n:%lu", line.c_str(), nums.size());
                            }
                            
                            int vertexIndex = atoi(nums[0].c_str());
                            int uvIndex     = atoi(nums[1].c_str());
                            int normalIndex = atoi(nums[2].c_str());
                            
                            if(vertexIndex <= 0 || vertexIndex > vertices.size()) {
                                Exit("Face vertex index out-of-bounds.");
                            }
                            
                            //printf("%f %f\n", uvs[uvIndex - 1].x, uvs[uvIndex - 1].y);
                            
                            VertexData face(
                                vertices[vertexIndex],
                                normals[normalIndex],
                                uvs[uvIndex]
                            );
                            
                            // Add to the current active model.
                            model->vertices.push_back(face);
                        }
                    }
               
                } else {
                    printf("%s\n", line.c_str());
                    Exit("Unknown .obj key: %s", key.c_str());
                }
            }
            
            // We add one to jump ahead of the "newline".
            readOffset = pos + 1;
        }
        
        printf("Loaded %lu models, total Vertices: %lu and UVs: %lu.\n", models.size(), vertices.size(), uvs.size());

        for(Model* model : models) {
            printf("  model: %s with %lu vertices.\n", model->group.c_str(), model->vertices.size());
            
            std::vector<unsigned short> indices;
        
            for(size_t i = 0, j = 0; i < model->vertices.size(); ++i) {
                model->indices.push_back((unsigned short)model->indices.size());
                
                // Random-ish color. This is usefull until textures work.
                model->vertices[i].color[0] = j * 10;
                model->vertices[i].color[1] = j * 30;
                model->vertices[i].color[2] = j * 7 + 60;
                
                if(i % 6 == 0) {
                    ++j;
                }
            }
        }
        
        return models;
    }
};

