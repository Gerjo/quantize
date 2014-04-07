//
//  PhotonGrid.h
//  Quantize
//
//  Created by Meri van Tooren on 05/04/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <limits>
#include <deque>
#include <vector>
#include <cmath>
#include "Math/Vector3.h"
#include "Math/Frmath.h"

using namespace Furiosity;

struct PhotonGridCell {
    std::deque<Photon> photons;
};

class PhotonGrid {
    static const int xRes = 5, yRes = 5, zRes = 5;
    static const int mass = xRes*yRes*zRes;
    PhotonGridCell grid[xRes][yRes][zRes];
    
    Vector3 lowLimit, highLimit;
    Vector3 interval = *new Vector3(0, 0, 0);
    
public:
    PhotonGrid(std::deque<Photon> photons, Vector3 lowLimit, Vector3 highLimit) {
        lowLimit = lowLimit;
        highLimit = highLimit;
        Vector3 d = highLimit - lowLimit;
        interval.x = d.x / xRes;
        interval.y = d.y / yRes;
        interval.z = d.z / zRes;
        
        Vector3 position;
        int xIndex, yIndex, zIndex;
        
        for (Photon p : photons) {
            position = p.position;
            position -= lowLimit;
            xIndex = (int)(position.x / interval.x);
            xIndex = Clamp(xIndex, 0, PhotonGrid::xRes - 1);
            yIndex = (int)(position.y / interval.y);
            yIndex = Clamp(yIndex, 0, PhotonGrid::yRes - 1);
            zIndex = (int)(position.z / interval.z);
            zIndex = Clamp(zIndex, 0, PhotonGrid::zRes - 1);
            grid[xIndex][yIndex][zIndex].photons.push_back(p);
        }
        
        printf("Grid built.");
    }
};