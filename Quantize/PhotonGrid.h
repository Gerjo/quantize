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

//struct PhotonGridCell {
//    std::deque<Photon> photons;
//};

class PhotonGrid {
public:
    static const int res = 50;

    static const int xRes = res, yRes = res, zRes = res;
    
    static const int mass = xRes*yRes*zRes;
    std::deque<Photon> grid[xRes][yRes][zRes];
    
    const Vector3 lowLimit;
    const Vector3 highLimit;
    Vector3 interval;
    
    const int photonCount;
    
public:
    PhotonGrid(const std::deque<Photon>& photons, const Vector3& lowLimit, const Vector3& highLimit)
        : lowLimit(lowLimit)
        , highLimit(highLimit)
        , interval(0, 0, 0)
        , photonCount((int)photons.size())
    {
    
        Vector3 d = highLimit - lowLimit;
        interval.x = d.x / xRes;
        interval.y = d.y / yRes;
        interval.z = d.z / zRes;
        
        size_t max = 0;
        
        for (const Photon& p : photons) {
            Vector3 position = p.position - lowLimit;
            int xIndex = Clamp(int(position.x / interval.x), 0, PhotonGrid::xRes - 1);
            int yIndex = Clamp(int(position.y / interval.y), 0, PhotonGrid::yRes - 1);
            int zIndex = Clamp(int(position.z / interval.z), 0, PhotonGrid::zRes - 1);
            
            grid[xIndex][yIndex][zIndex].push_back(p);
            
            max = std::max(max, grid[xIndex][yIndex][zIndex].size());
        }
        
        printf("Grid built. Volume: %d cells. Highest occupancy: %zu\n", mass, max);
    }
    
    std::vector<float> toVector() {

        // Volume of the grid in floats, each cell is a vector of 3.
        const int photonOffset = mass * (sizeof(Vector3) / sizeof(float));

        // Number of floats in a photon
        const int photonSize = sizeof(Photon) / sizeof(float);

        // I assume that each photon only exists in one cell.
        std::vector<float> result(
            // Size calculation
            photonOffset + photonCount * photonSize,
            
            // Default value
            0.0f
        );
        
        
        
        int gridIndex    = 0;
        int photonIndex  = photonOffset;
        
        for(int x = 0; x < xRes; ++x) {
            for(int y = 0; y < yRes; ++y) {
                for(int z = 0; z < zRes; ++z) {
                    int count = (int) grid[x][y][z].size();
                    
                    // Count in this cell stored in de X component
                    result[gridIndex + 0] = count;

                    // Starting index stored in de Y component
                    result[gridIndex + 1] = photonIndex / photonSize;
                    
                    // Copy each photon into the output array
                    for(int j = 0; j < count; ++j) {
                    
                        // Such memory hack, many photon wow float.
                        float* floaton = (float*) & grid[x][y][z][j];
                        for(int i = 0; i < photonSize; ++i) {
                            result[photonIndex] = floaton[i];
                            
                            ++photonIndex;
                        }
                    }
                    
                    // Photons are n bytes, make sure the buffer contains a
                    // multiple of n bytes. If not, this indicates a photon
                    // was only copied partically.
                    assert((photonIndex - photonOffset) % photonSize == 0);
                    
                    // Test for memory overflow
                    assert(photonIndex <= result.size());
                    
                    // End index stored in de Z component
                    result[gridIndex + 2] = photonIndex / photonSize;
                    
                    // Offset by a vector
                    gridIndex += 3;
                }
            }
        }
        
        
        
        
        printf("Float size: %zu, pixel size: %zu\n", result.size(), result.size()/3);
        
        return result;
    }
};
