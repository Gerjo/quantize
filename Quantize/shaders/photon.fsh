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

// Used for randomness
uniform int frameCounter;
uniform float time;

uniform sampler2D zdata;        // Triangles packed in texture
uniform int triangleCount;      // Number of triangles

uniform int mapWidth;
uniform int mapHeight;


uniform vec2 windowSize;


// Write new photon into:
out vec3 outDirection;
out vec3 outPosition;
out vec3 outMeta;

// Read from photon from:
uniform sampler2D inBuffers[3];  // Direction, position and meta.

uniform int lightCount;          // Amount of light sources
uniform vec3 lightPositions[5];  // Position of each light, up to 10.

const int stride     = 9;        // In vec3
const int lod        = 0;        // mipmap level

uniform sampler2D textures[11];


void main() {
    
    srand(uvmapping.x + uvmapping.y * 100);
    
    ivec2 texelIndex = ivec2(
        uvmapping.x * windowSize.x,
        uvmapping.y * windowSize.y
    );
    
    vec3 inDirection = texelFetch(inBuffers[0], texelIndex, lod).xyz;
    vec3 inPosition  = texelFetch(inBuffers[1], texelIndex, lod).xyz;
    vec3 inMeta      = texelFetch(inBuffers[2], texelIndex, lod).xyz;
    
    //vec3 inDirection = texture(inBuffers[0], uvmapping).xyz;
    //vec3 inPosition  = texture(inBuffers[1], uvmapping).xyz;
    //vec3 inMeta      = texture(inBuffers[2], uvmapping).xyz;
    
    
    bool isAlive = int(inMeta.x) == 1;
    int bounces  = int(inMeta.z);
    
    if( ! isAlive) {
        // Transfer the dead meta state.
        outMeta      = vec3(0, 0, bounces + 1);
        outPosition  = vec3(0, 0, 0);
        outDirection = vec3(0, 0, 0);
        return;
    }
    
    // Russian roulette after the first bounce
    if(bounces > 0) {
        if(rand() > 0.95) { // Kill n%
            outMeta = vec3(
                    0,           // dead.
                    0,           // no color
                    bounces + 1  // bounces
            );
            
            return;
        }
    }
    
    // Zero initialize.
    outDirection = vec3(9, 9, 9);
    outPosition  = vec3(0, 0, 0);
    outMeta      = vec3(
                  1,            // Alive.
                  0,            // Color
                  bounces + 1   // Number of bounces
                  
    );

    //outPosition = vec4(inDirection,  0);
    //return;

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
        bool collision = rayIntersetsTriangle(ray, A, B, C, true, where, depth);
        
        if( ! collision) {
        
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
        
        vec2 uv     = barycentric(bestHitPosition, A, B, C, U, V, W);
        int sampler = int(texelFetch(zdata, ivec2(bestHitOffset + 3, 0), lod).z);
        
        vec4 color = texture(textures[sampler], uv);
        //color = vec4(1, 1, 0, 1);//
        
        // Pack color into integer. msb (rrrrrrrr gggggggg bbbbbbbb aaaaaaaa) lsb
        int intColor =  ((int(color.r * 255) & 255) << 24) |
                        ((int(color.g * 255) & 255) << 16) |
                        ((int(color.b * 255) & 255) <<  8) |
                        ((int(color.a * 255) & 255) <<  0) ;
        
        outMeta.y = intColor;
        
        // Find normal
        vec3 normal = normalize(barycentric3(bestHitPosition, A, B, C, n1, n2, n3));
        
        // Reflect about the surface normal.
        outDirection = reflect(ray.direction, normal);
        outPosition  = bestHitPosition;
    } else {
        // Hit nothing, mark as "dead".
        outMeta.x = 0;
    }
    
}
