
//
//  KdTree.h
//  Quantize
//
//  Created by Gerard Meier on 25/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

enum axis{X, Y, Z};

struct Photon {
    Vector3 color;
    Vector3 position;
    Vector3 meta;
    
    // This _works_ for now.
    Photon(const float* positionData, const float* colorData, const float* metaData) {
        color.x = positionData[0];
        color.y = positionData[1];
        color.z = positionData[2];
        position.x = colorData[0];
        position.y = colorData[1];
        position.z = colorData[2];
        meta.x = metaData[0];
        meta.y = metaData[1];
        meta.z = metaData[2];
    }
};

struct Node {
    struct Node* alpha;
    struct Node* beta;
    int splitAxis;
    float pivot;
};

class KdTree {

    std::vector<Photon> storage;
    struct Node* tree = new struct Node;

    struct Node* buildTree(std::deque<Photon> photons, int splitAxis) {
        struct Node* node = new struct Node;
        node->splitAxis = splitAxis;
        
        std::deque<Photon> alphaSet, betaSet;
        
        float pivot = 0.0f;
        
        // Find median of all photons
        
        // pivot = median
        
        // divide set into < pivot and > pivot
        for (Photon p : photons) {
            if (splitAxis == X) {
                if (p.position.x < pivot)
                    alphaSet.push_back(p);
                else
                    betaSet.push_back(p);
            }
            else if (splitAxis == Y) {
                if (p.position.y < pivot)
                    alphaSet.push_back(p);
                else
                    betaSet.push_back(p);
            }
            else {
                if (p.position.z < pivot)
                    alphaSet.push_back(p);
                else
                    betaSet.push_back(p);
            }
        }
        
        // recursion with sets to make alpha and beta
        node->alpha = buildTree(alphaSet, (++splitAxis) % 3);
        node->beta = buildTree(betaSet, (++splitAxis) % 3);
        
        return node;
    }
    
public:
    KdTree(std::deque<Photon> photons) {
        tree->splitAxis = X;
        
        // And then some.
    }
};

