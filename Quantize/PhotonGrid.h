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
    static const int res = 40;

    static const int xRes = res, yRes = res, zRes = res;
    
    static const int mass = xRes*yRes*zRes;
    std::deque<Photon> grid[xRes][yRes][zRes];
    
    Vector3 lowLimit;
    Vector3 highLimit;
    Vector3 interval;
    
    const int photonCount;
    
public:
    PhotonGrid(std::deque<Photon> photons)
        : interval(0, 0, 0)
        , photonCount((int)photons.size())
        , lowLimit( +std::numeric_limits<float>::infinity())
        , highLimit(-std::numeric_limits<float>::infinity())
    {
    
        // Compute axis aligned bounding volume
        for(const Photon& p : photons) {
            lowLimit.x  = std::min(lowLimit.x, p.position.x);
            lowLimit.y  = std::min(lowLimit.y, p.position.y);
            lowLimit.z  = std::min(lowLimit.z, p.position.z);
            
            highLimit.x = std::max(highLimit.x, p.position.x);
            highLimit.y = std::max(highLimit.y, p.position.y);
            highLimit.z = std::max(highLimit.z, p.position.z);
        }
    
        highLimit(20, 20, 20);
        lowLimit(-20, -20, -20);
    
        printf("From: %.4f %.4f %.4f\n", lowLimit.x, lowLimit.y, lowLimit.z);
        printf("To:   %.4f %.4f %.4f\n", highLimit.x, highLimit.y, highLimit.z);
    
        Vector3 range = highLimit - lowLimit;
        interval.x = range.x / (xRes-1); // Remove one to make left-inclusive [n..n)
        interval.y = range.y / (yRes-1);
        interval.z = range.z / (zRes-1);
        
        size_t max = 0;
        
        for (Photon p : photons) {
            
            // Offset to a [0...n] range
            Vector3 position = p.position - lowLimit;
            
            // Quantize
            int xIndex = std::round(position.x / interval.x);
            int yIndex = std::round(position.y / interval.y);
            int zIndex = std::round(position.z / interval.z);
            
            assert(xIndex >= 0); assert(xIndex < xRes);
            assert(yIndex >= 0); assert(yIndex < yRes);
            assert(zIndex >= 0); assert(zIndex < zRes);
            
            //p.direction(1,1,1);
            //p.position(2,2,2);
            //p.meta(3,3,3);
            
            grid[xIndex][yIndex][zIndex].push_back(p);
            
            max = std::max(max, grid[xIndex][yIndex][zIndex].size());
        }
        
        printf(" done. Volume: %d cells. Max occupancy: %zu\n", mass, max);
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
        
        printf("Resulution: %dx%dx%d\n", xRes, yRes, zRes);
        
        for(int x = 0; x < xRes; ++x) {
            for(int y = 0; y < yRes; ++y) {
                for(int z = 0; z < zRes; ++z) {
                
                    
                    int cellIndex = z +
                            (y * xRes)
                            +
                            (x * xRes * yRes);
                    
                    assert(cellIndex == gridIndex/3);
                
                    //printf("Grid: [index: %d, %dx%dx%d] -> [compute: %d] - quantized.\n", gridIndex/3, x,y,z, cellIndex);
                
                    std::sort(grid[x][y][z].begin(), grid[x][y][z].end(), [] (const Photon& a, const Photon& b) {
                        return a.meta.z > b.meta.z;
                    });
                
                    int count = (int) grid[x][y][z].size();
                    
                    // Count in this cell stored in de X component
                    result[gridIndex + 0] = count;

                    // Starting index stored in de Y component
                    result[gridIndex + 1] = photonIndex / 3;
                   
                    
                    // Copy each photon into the output array
                    for(int j = 0; j < count; ++j) {
                    
                        // quantize.
                        int xIndex = std::round((grid[x][y][z][j].position.x - lowLimit.x) / interval.x);
                        int yIndex = std::round((grid[x][y][z][j].position.y - lowLimit.y) / interval.y);
                        int zIndex = std::round((grid[x][y][z][j].position.z - lowLimit.z) / interval.z);
                    
                        if(xIndex != x) {
                            printf("%d != %d\n", xIndex, x);
                            assert(xIndex == x);
                        }
                        if(yIndex != y) {
                            printf("%d != %d\n", yIndex, y);
                            assert(yIndex == y);
                        }
                        if(zIndex != z) {
                            printf("%d != %d\n", zIndex, z);
                            assert(zIndex == z);
                        }
                        
                    
                        //if(cellIndex != gridIndex/3) {
                        //    printf("Grid: [index: %d, %dx%dx%d] -> [compute: %d] - quantized.\n", gridIndex/3, x,y,z, cellIndex);
                          //  assert(cellIndex == gridIndex/3);
                        //}
                    
                        // Such memory hack, many photon wow float.
                        float* floaton = grid[x][y][z][j].direction.v;
                        for(int i = 0; i < photonSize; ++i) {
                        
                           
                        
                            result[photonIndex] = floaton[i];
                            
                            ++photonIndex;
                 
                        }
                    }
                    
                    // Photons are n bytes, make sure the buffer contains a
                    // multiple of n bytes. If not, this indicates a photon
                    // was only copied partically.
                    assert(photonOffset % 3 == 0);
                    
                    // Test for memory overflow
                    assert(photonIndex <= result.size());
                    
                    // End index stored in de Z component
                    result[gridIndex + 2] = photonIndex / 3;
                    
                    //if(count != 0)
                        //printf("Start: %.0f End: %.0f, delta: %.0f == %d\n", result[gridIndex + 1], result[gridIndex + 2],
                        
                       // result[gridIndex + 2] - result[gridIndex + 1], count * 3);
                    
                    // Offset by a vector
                    gridIndex += 3;
                }
            }
        }
        
        
        auto foo = result;
        
        /*for(int i = 0; i < result.size(); ++i) {
            printf("result[%d] = %.0f\n", i, result[i]);

            if((i+1) % 3 == 0) {
                printf("--\n");
            }
        }*/
        
        printf("Float size: %zu, pixel size: %zu\n", result.size(), result.size()/3);
        
        return result;
    }
};
