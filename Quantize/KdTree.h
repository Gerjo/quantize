
//
//  KdTree.h
//  Quantize
//
//  Created by Gerard Meier on 25/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once


#include <limits>
#include <deque>
#include <vector>
#include <cmath>
#include "Math/Vector3.h"

using namespace Furiosity;

struct Photon {
    Vector3 color;
    Vector3 position;
    Vector3 meta;
    
    // This _works_ for now.
    Photon(const float* positionData, const float* colorData, const float* metaData) {
        color.x = colorData[0];
        color.y = colorData[1];
        color.z = colorData[2];
        position.x = positionData[0];
        position.y = positionData[1];
        position.z = positionData[2];
        meta.x = metaData[0];
        meta.y = metaData[1];
        meta.z = metaData[2];
    }
    
    Photon() = default;
};

struct Node {
    /// Left branch
    Node* alpha;
    
    /// Right branch
    Node* beta;
    
    /// Photon at this node.
    Photon photon;
    
    /// Splitting axis
    const int axis;
    
    /// Recursive constructor, expands the tree.
    Node(std::deque<Photon> array, int axis);
    
    /// Default ctor does not use recursion.
    Node() : axis(0) {
        alpha = nullptr;
        beta  = nullptr;
        
        // Promote to infinity node.
        photon.position.x = std::numeric_limits<float>::infinity();
        photon.position.y = std::numeric_limits<float>::infinity();
        photon.position.z = std::numeric_limits<float>::infinity();
    }
    
    /// Recursively destroy this node and its branch.
    virtual ~Node() {
        delete alpha;
        delete beta;
        
        alpha = nullptr;
        beta = nullptr;
    }
};

class KdTree {
    Node* root;
    
public:
    KdTree(std::deque<Photon> photons)
    :  root(new Node(photons, 0))
    {
    }
    
    ~KdTree() {
        delete root;
        root = nullptr;
    }
    
    std::vector<Photon> toVector() {
        std::vector<Photon> v;
        
        std::deque<Node*> queue;
        
        queue.push_front(root);
        
        while( ! queue.empty()) {
            Node* node = queue.front();
            queue.pop_front();
            
            if(node->alpha) {
                queue.push_back(node->alpha);
            }
            
            if(node->beta) {
                queue.push_back(node->beta);
            }
            
            v.push_back(node->photon);
        }
        
        // Remove trailing infinity photons. This is a space-saving thing, when
        // a photon out-of-bounds index is requested - we can assume it's an
        // infinity node.
        while( ! v.empty() && std::isinf(v.back().position.x)) {
            v.pop_back();
        }
        
        return v;
    }
};

