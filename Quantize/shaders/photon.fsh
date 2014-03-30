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

uniform int mapWidth;
uniform int mapHeight;

uniform vec2 windowSize;


// Write new photon into:
out vec4 outDirection;
out vec4 outPosition;
out vec4 outMeta;

// Read from photon from:
uniform sampler2D inBuffers[3];  // Direction, position and meta.

uniform int lightCount;          // Amount of light sources
uniform vec3 lightPositions[5];  // Position of each light, up to 10.

const int stride     = 9;        // In vec3
const int lod        = 0;        // mipmap level

uniform sampler2D textures[14];

// Some initial seed.
int seed = 10;

void srand(float s) {
    seed = int(s * 12345);
}

float rand() {
    seed = (1103515245 * seed + 12345) % 2147483648;

    return fract(seed * 0.001);
}

void main() {
    
    vec3 inDirection = texture(inBuffers[0], uvmapping).xyz;
    vec3 inPosition  = texture(inBuffers[1], uvmapping).xyz;
    vec3 inMeta      = texture(inBuffers[2], uvmapping).xyz;
    
    // Zero initialize.
    outDirection = vec4(1, 0, 0, 1);
    outPosition  = vec4(0, 0, 0, 0);
    outMeta      = vec4(
                  1,            // Alive? apply Russian roulette?
                  0,            // Color?
                  inMeta.z + 1, // Number of bounces
                  0             // Homogenious.
    );

    Ray ray;
    
    // Start at a given light source position
    ray.place = inPosition;
    
    // Follow the direction
    ray.direction = inDirection;

    
    int hits             = 0;
    int bestHitOffset    = 0;
    float bestHitDepth   = Infinity;
    vec3 bestHitPosition = vec3(0, 0, 0);
    
    
    vec3 where  = vec3(0, 0, 0);
    float depth = 0.0f;
    
    // Test against world.
    for(int i = 0; i < triangleCount; ++i) {
        int offset = i * stride;
        
        vec3 A = texelFetch(zdata, ivec2(offset + 0, 0), lod).xyz;
        vec3 B = texelFetch(zdata, ivec2(offset + 1, 0), lod).xyz;
        vec3 C = texelFetch(zdata, ivec2(offset + 2, 0), lod).xyz;
        
        
        // Ray collision test; "where" is an output: the point of intersection.
        int res = rayIntersetsTriangle(ray, A, B, C, true, where, depth);
        
        if(res != 0) {
        
            // Keep only the nearest surface. We've simplified the model to not
            // use z-buffers.
            if(depth < bestHitDepth) {
                bestHitDepth    = depth;
                bestHitPosition = where;
                bestHitOffset   = offset;
            }
      
            ++hits;
        }
    }
    
    if(hits > 0) {
        // Triangle vertices
        vec3 A = texelFetch(zdata, ivec2(bestHitOffset + 0, 0), lod).xyz;
        vec3 B = texelFetch(zdata, ivec2(bestHitOffset + 1, 0), lod).xyz;
        vec3 C = texelFetch(zdata, ivec2(bestHitOffset + 2, 0), lod).xyz;
        
        // UV blending:
        vec2 U = texelFetch(zdata, ivec2(bestHitOffset + 3, 0), lod).xy;
        vec2 V = texelFetch(zdata, ivec2(bestHitOffset + 4, 0), lod).xy;
        vec2 W = texelFetch(zdata, ivec2(bestHitOffset + 5, 0), lod).xy;
        
        // Normal blending:
        vec3 n1 = texelFetch(zdata, ivec2(bestHitOffset + 6, 0), lod).xyz;
        vec3 n2 = texelFetch(zdata, ivec2(bestHitOffset + 7, 0), lod).xyz;
        vec3 n3 = texelFetch(zdata, ivec2(bestHitOffset + 8, 0), lod).xyz;
        
        // Solve for UV coordinates
        /*
        vec2 uv     = barycentric(bestHitPosition, A, B, C, U, V, W);
        int sampler = int(texelFetch(zdata, ivec2(bestHitOffset + 3, 0), lod).z);
        outDirection    = texture(textures[sampler], uv);
        */
        
        // Find normal
        vec3 normal = barycentric3(bestHitPosition, A, B, C, n1, n2, n3);
        
        // Reflect about the surface normal. TODO: not sure if this "reflect" works
        // as expected.
        outDirection = vec4(reflect(ray.direction, normal), 8);
    }
    
    outPosition = vec4(bestHitPosition, 1);
}
