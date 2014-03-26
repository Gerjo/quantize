
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
    
    Photon() {
        
    }
};

struct Node {
    struct Node* alpha;
    struct Node* beta;
    Photon photon;
    int splitAxis;
    float pivot;
    bool leaf;
};

class KdTree {
    struct Node* tree;
    
    static bool comparatorX(Photon a, Photon b) {
        return (a.position.x < b.position.x);
    }
    
    static bool comparatorY(Photon a, Photon b) {
        return (a.position.y < b.position.y);
    }
    
    static bool comparatorZ(Photon a, Photon b) {
        return (a.position.z < b.position.z);
    }
    
    static Photon median(std::deque<Photon> photons, int splitAxis) {
        switch (splitAxis) {
            case X:
                std::sort(photons.begin(), photons.end(), comparatorX);
                break;
            case Y:
                std::sort(photons.begin(), photons.end(), comparatorY);
                break;
            case Z:
                std::sort(photons.begin(), photons.end(), comparatorZ);
                break;
        }
        int index = (int)photons.size() / 2;
        return photons[index];
    }

    struct Node* buildTree(std::deque<Photon> photons, int splitAxis) {
        struct Node* node = new struct Node;
        node->splitAxis = splitAxis;
        if (photons.size() == 1) {
            node->leaf = true;
            node->photon = photons[0];
            return node;
        }
        else
            node->leaf = false;
        
        std::deque<Photon> alphaSet, betaSet;
        
        // find the median
        Photon pivot = median(photons, splitAxis);
        node->photon = pivot;
        
        // divide set into < pivot and > pivot
        for (Photon p : photons) {
            if (splitAxis == X) {
                node->pivot = pivot.position.x;
                if (p.position.x < pivot.position.x)
                    alphaSet.push_back(p);
                else
                    betaSet.push_back(p);
            }
            else if (splitAxis == Y) {
                node->pivot = pivot.position.y;
                if (p.position.y < pivot.position.y)
                    alphaSet.push_back(p);
                else
                    betaSet.push_back(p);
            }
            else {
                node->pivot = pivot.position.z;
                if (p.position.z < pivot.position.z)
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
        tree = buildTree(photons, X);
        
        // And then some.
    }
    
    std::vector<Photon> toVector() {
        std::deque<Node> inOrder;
        std::deque<Node> queue;
        
        while(true) {
            while(queue.size() > 0) {
                //pop node
                
                //if not leaf, enqueue children
                
                //inOrder.push_back(node);
            }
        }
        
        //inOrder -> vector<Photon>
    }
};

