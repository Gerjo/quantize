
//
//  KdTree.h
//  Quantize
//
//  Created by Gerard Meier on 25/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

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

class KdTree {

    std::vector<Photon> storage;

public:
    KdTree(std::deque<Photon> photons) {
        // And then some.
    }
};

