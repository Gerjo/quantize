//
//  Tree.h
//  Quantize
//
//  Created by Gerard Meier on 14/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Math/Vector3.h"
#include "Tribox.h"
#include "Tools.h"
#include "Model.h"

#include <array>

/// Do not partition below this size. TODO: refine this approach.
const float minSize     = 0.1f;     // Never partition smaller than this.
const int   maxChildren = 5;        // Only partition if a node has more than x children
const float initialSize = 5.0f;     // Starting hsize of the tree.


int triangleId = 0; // Counter to assign each triangle a unique ID. For debugging.

using namespace Furiosity;

/// TODO: pass this to the GPU and remove some redundancies.
struct Triangle {
    VertexData a;
    VertexData b;
    VertexData c;
    
    float* triangle[3];
    
    int id;
};

/// Box i.e., leafe
struct Box {
    Vector3 center;
    Vector3 hsize;
    
    std::array<Box*, 8> boxes{{nullptr}};
    
    std::vector<Triangle> triangles;
    
    Box() : center(0, 0, 0), hsize(0, 0, 0) {
    
    }
    
    bool isLeaf() const {
        return boxes[0] == nullptr;
    }
    
    void partition() {
    
        if(! isLeaf() ) {
            Exit("Cannot partition leafe, it is already partitioned.");
        }
    
        Vector3 qsize = hsize * 0.5;
        
        // An artist's rendering of this box (not to scale):
        //
        //      +------+------+
        //     /  4   /  7   / |
        //    /_____ /______/  |
        //   /      /      /| 7|
        //  /  0   /   3  / | /|
        // +------+------+  |/ |
        // |  0   +  3   |3 / 6|
        // |      +      | /| /
        // +------+------+/ |/
        // |  1   +  2   |2 /
        // |      +      | /
        // +------+------+/
        //
        //
        
        for(int i = -1, j = 0; i < 2; i += 2, j += 4) {
            boxes[0 + j] = new Box();
            boxes[0 + j]->hsize = qsize;
            boxes[0 + j]->center = center;
            boxes[0 + j]->center.x -= qsize.x;
            boxes[0 + j]->center.y += qsize.y;
            boxes[0 + j]->center.z += qsize.z * i;
            
            boxes[1 + j] = new Box();
            boxes[1 + j]->hsize = qsize;
            boxes[1 + j]->center = center;
            boxes[1 + j]->center.x -= qsize.x;
            boxes[1 + j]->center.y -= qsize.y;
            boxes[1 + j]->center.z += qsize.z * i;
            
            boxes[2 + j] = new Box();
            boxes[2 + j]->hsize = qsize;
            boxes[2 + j]->center = center;
            boxes[2 + j]->center.x += qsize.x;
            boxes[2 + j]->center.y -= qsize.y;
            boxes[2 + j]->center.z += qsize.z * i;
            
            boxes[3 + j] = new Box();
            boxes[3 + j]->hsize = qsize;
            boxes[3 + j]->center = center;
            boxes[3 + j]->center.x += qsize.x;
            boxes[3 + j]->center.y += qsize.y;
            boxes[3 + j]->center.z += qsize.z * i;
        }
        
         // Disburse triangles
        for(size_t i = 0; i < boxes.size(); ++i) {
            for(Triangle t : triangles) {
                boxes[i]->insert(t);
            }
        }
    }
    
    void insert(Triangle triangle) {
  
        int r = triBoxOverlap(center.v, hsize.v, triangle.triangle);
        
        if(r == 1) {
            
            if(isLeaf() && hsize.x > minSize) {
                
                if(triangles.size() > maxChildren) {
                    partition();
                }
            }
            
            triangles.push_back(triangle);
            
            if( ! isLeaf()) {
                for(size_t i = 0; i < boxes.size(); ++i) {
                    boxes[i]->insert(triangle);
                }
            }
        } else {
            //printf("Not today. hsize: %f %f %f \n", hsize.x, hsize.y, hsize.z);
        }
    }
    
    size_t size() {
        size_t r(0);
        
        if( ! isLeaf() && triangles.size() > 0) {
            for(size_t i = 0; i < boxes.size(); ++i) {
                r += boxes[i]->size();
            }
        }
        
        // TODO: refine this. It's probably wrong.
        return r + sizeof(VertexData) * triangles.size() * 3;
    }
    
    void print(int depth) {
    
        std::string padding; padding.assign(depth * 3, ' ');
        
        if( ! triangles.empty()) {
        
            std::string s;
        
            for(Triangle t : triangles) {
                s = s + std::to_string(t.id) + ", ";
            }
        
            printf("%sChildren: %lu [%s]\n", padding.c_str(), triangles.size(), s.substr(0, s.length() - 2).c_str());
        }
        
        if( ! isLeaf()) {
            for(size_t i = 0; i < boxes.size(); ++i) {
                boxes[i]->print(depth + 1);
            }
        }
    }
};

class Tree {
public:
    Box root;
    
    Tree() {
        // Half size.
        root.hsize(initialSize, initialSize, initialSize);
        
        // This is a center based tree (todo: center singularity).
        root.center(0.0, 0.0, 0.0);
    }
    
    /// Insert a vertexdata triplet (triangle) into this tree.
    void insert(VertexData v1, VertexData v2, VertexData v3) {
  
        // Ad-hoc create a trianagle.
        Triangle t{v1, v2, v3, {
                v1.position.v,
                v2.position.v,
                v3.position.v
            }, ++triangleId
        };
    
        // Insert into the root, let recursion follow.
        root.insert(t);
    }
    
    /// Print debug representation of this tree.
    void print() {
        root.print(0);
    };
    
    size_t size() {
        return root.size();
    }
};
