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

const int stride     = 6;        // In vec3
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
    
    // Zero initialize.
    outColor     = vec4(1, 0, 0, 1);
    outPosition  = vec4(0, 0, 0, 0);
    outMeta      = vec4(9, 9, 9, 9);

    int lightSource = 0;


    srand(pixelPosition.y + pixelPosition.x * 100);

    Ray ray;
    
    // Start at a given light source position
    ray.place = vec3(0, 1, 0);//lightPositions[lightSource];
    
    // Random direction, this should be derived from the projection map.
    ray.direction = normalize(vec3(
        rand() - 0.5,
        rand() - 0.5,
        rand() - 0.5
    ));
    
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
            outPosition = vec4(where, 1);
            
            // Uv coordinates per triangle vertex
            vec2 U = texelFetch(zdata, ivec2(offset + 3, 0), lod).xy;
            vec2 V = texelFetch(zdata, ivec2(offset + 4, 0), lod).xy;
            vec2 W = texelFetch(zdata, ivec2(offset + 5, 0), lod).xy;
        
            // Solve for UV coordinates
            vec2 uv = barycentric(where, A, B, C, U, V, W);
            
            int sampler = int(texelFetch(zdata, ivec2(offset + 3, 0), lod).z);
            outColor = texture(textures[sampler], uv);
            
            
            
            // Take note of incomming angle and so-called "power". These are
            // to be stored into outMeta as a vec4.
            
            // We can experiment with colors too, these can be stored in
            // outColor as vec4.
            
            ++hits;
            break;
        }
    }
}
