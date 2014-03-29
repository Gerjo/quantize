//
//  KdTree.cpp
//  Quantize
//
//  Created by Gerard Meier on 29/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "KdTree.h"


Node::Node(std::deque<Photon> array, int axis)
    : axis(axis)
    , alpha(new Node())
    , beta(new Node())
{
    int axes = 3; // Number of vector components.
    
    // Sort on axis
    std::sort(array.begin(), array.end(), [axis] (const Photon& u, const Photon& v) {
        return u.position[axis] < v.position[axis];
    });
    
    // Middle index
    int middle = array.size() * 0.5;
    
    // Median photon
    photon = array[middle];
    
    std::deque<Photon> a, b;
    
    // Split array into two segments
    for(size_t i = 0; i < array.size(); ++i) {
        if(i < middle) {
            a.push_back(array[i]);
        } else if(i > middle) {
            b.push_back(array[i]);
        }
    }
    
    // Expand the left branch
    if( ! a.empty()) {
        alpha = new Node(a, (axis + 1) % axes);
    }
    
    // Expand the right branch
    if(! b.empty()) {
        beta = new Node(b, (axis + 1) % axes);
    }
}