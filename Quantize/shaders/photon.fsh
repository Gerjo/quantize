//
//  photon.fsh
//
//  Created by Gerard Meier on 18/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "shaders/common.glsl"

in vec2 uvmapping;
in vec2 uvunit;

in vec2 localPosition;
in vec2 pixelPosition;


uniform sampler2D zdata;        // Triangles packed in texture
uniform int triangleCount;      // Number of triangles

// Previous bounce
uniform vec4 uniformColor;
uniform vec4 uniformPosition;
uniform vec4 uniformMeta;

// New bounce
out vec4 outColor;
out vec4 outPosition;
out vec4 outMeta;

uniform int lightCount;           // Amount of light sources
uniform vec3 lightPositions[10];  // Position of each light, up to 10.

const float Infinity = 99999999; // Infinity, for all intents and purposes.
const int stride     = 6;        // In vec3
const int lod        = 0;        // mipmap level


void main() {
    
    // Zero initialize.
    outColor     = vec4(1, 2, 3, 3);
    outPosition  = vec4(0, 0, 0, 0);
    outMeta      = vec4(9, 9, 9, 9);

    int lightSource = 0;


    Ray ray;
    
    // Start at a given light source position
    ray.place = vec3(0, 0, 0);//lightPositions[lightSource];
    
    // Random direction, this should be derived from the projection map.
    ray.direction = vec3(random(pixelPosition) * 3.14 - 0.5, random(pixelPosition) * 2.71 - 0.5, random(pixelPosition) * 1.65 - 0.5);
    
    // Normalize, for good measure.
    ray.direction = normalize(ray.direction);
        
    // Test against world.
    
    int hits = 0;
    for(int i = 0; i < triangleCount && hits == 0; ++i) {
        int offset = i * stride;
        
        vec3 A = texelFetch(zdata, ivec2(offset + 0, 0), lod).xyz;
        vec3 B = texelFetch(zdata, ivec2(offset + 1, 0), lod).xyz;
        vec3 C = texelFetch(zdata, ivec2(offset + 2, 0), lod).xyz;
        
        vec3 where = vec3(0, 0, 0);
        float depth = 0.0f;
        
        // Ray collision test; "where" is an output: the point of intersection.
        int res = rayIntersetsTriangle(ray, A, B, C, true, where, depth);
        
        if(res != 0) {
            outPosition = vec4(pixelPosition, 1, 1);
            
            // Take note of incomming angle and so-called "power". These are
            // to be stored into outMeta as a vec4.
            
            // We can experiment with colors too, these can be stored in
            // outColor as vec4.
            
            ++hits;
            break;
        }
    }
}
