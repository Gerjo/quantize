//
//  raytracer.vsh
//
//  created by Gerard Meier on 19/02/14.
//  copyright (c) 2014 Quantize. All rights reserved.
//

#include "shaders/common.glsl"

uniform int lightCount;
uniform vec3 lightsPosition[10];
uniform vec4 lightsDiffuse[10];
uniform vec4 lightsSpecular[10];
uniform vec4 lightsAmbiant[10];

uniform vec2 windowSize;        // Size of the viewport
in vec2 position;               // Normalize position on screen
in vec2 pixelPosition;          // The approximate pixel on screen.

uniform int numTriangles;       // Number of triangles

uniform int useTexture;

uniform int n;
uniform float sigma;
uniform float range;
uniform int enableJitter;

// Some transforms
uniform mat4 translation;
uniform mat4 rotation;

// Used for randomness
uniform int frameCounter;
uniform float time;

uniform int mapWidth;
uniform int mapHeight;


// My Intel onboard chip only supports 16 textures. If this becomes a limit,
// we can make an atlas - textures up to 16k resolution are supported. Reading
// the spec further, we can use a sampler2DArray!
uniform sampler2D textures[14];


/// Format (non optimized structure, directly copied from c++)
/// [0] Vector3 position;
/// [1] Vector3 normal;
/// [2] Vector2 uv;
/// [2] unsigned char color[4];
/// [3] GLuint sampler;
/// [3] Glbyte[2] padding;
uniform sampler2D zdata;

const int stride     = 9;        // In vec3
const int lod        = 0;        // mipmap level

out vec4 finalColor;

uniform sampler2D photons;
uniform int numPhotons; // Number of photons

struct Photon {
    vec3 direction;
    vec3 position;
    vec3 meta;
};

ivec2 photonIndex(in int index, in int offset) {

    // Each photon has 3 vectors (direction, position and meta)
    const int stride = 3;
    
    int i = index * stride + offset;
    
    // The usual index to grid coordinate routine.
    int y = i / mapWidth;
    int x = i - y * mapWidth;
    
    return ivec2(x, y);
}

Photon nearestPhoton(in vec3 search) {
    const int axes = 3;
    
    int index = 1;
    int axis  = 0;
    
    int bestIndex = 0;
    float bestDistance = Infinity;
    vec3 bestPosition = vec3(0, 0, 0);
    
    while(index <= numPhotons) {
        ivec2 texelIndex = photonIndex(index - 1, 1/*position offset*/);
        vec3 tentative = texelFetch(photons, texelIndex, lod).xyz;
        float distance = dot(length(tentative - search), length(tentative - search));
        
        if(distance < bestDistance) {
            bestIndex    = index;
            bestDistance = distance;
            bestPosition = tentative;
        }
        
        if(search[axis] > tentative[axis]) {
            index = index * 2 + 1;
        } else {
            index = index * 2;
        }
        
        axis = (axis + 1 ) % axes;
    }

    Photon p;
    p.position = bestPosition;
    p.meta = texelFetch(photons, photonIndex(bestIndex - 1, 2/*meta offset*/), lod).xyz;
    p.direction = texelFetch(photons, photonIndex(bestIndex - 1, 0/*meta offset*/), lod).xyz;

    return p;
}

///   ---> direction --->
///   eye       canvas            object
///       /      |
///      /       |                 +------+
///     /        |                 |      |
///   #          |                 |      |
///     \        |                 +------+
///      \       |
///       \      |

vec4 traceRay(in vec2 pos, in float perspective) {
    
    
    Ray ray;
    
    // Camera at origin
    ray.place = vec3(0, 0, 0);
    
    // Offset canvas from the camera (substraction is useless, but here for competeness)
    ray.direction = normalize(vec3(pos, ray.place.z + perspective) - ray.place);
    
    // Rotate direction about camera
    ray.direction = (rotation * vec4(ray.direction, 1.0)).xyz;
    
    // Translate the camera
    ray.place -= vec3(translation[3][0] * 0.1, translation[3][1] * 0.1, translation[3][2] * 0.1);
    
    const int maxBuffer = 8; // weird transparent object? increase this number.
    vec4 zBufferColor[maxBuffer];
    float zBufferDepth[maxBuffer];
    
    float infLightCount = 1.0 / lightCount;
    float ambientRatio  = 0.4 * infLightCount;
    
    int j = 0;
    for(int i = 0; i < numTriangles; ++i) {
        
        int offset = i * stride;
        vec3 A = texelFetch(zdata, ivec2(offset + 0, 0), lod).xyz;
        vec3 B = texelFetch(zdata, ivec2(offset + 1, 0), lod).xyz;
        vec3 C = texelFetch(zdata, ivec2(offset + 2, 0), lod).xyz;
        
        vec3 where;
        float depth;
        
        // Ray collision test; "where" is an output: the point of intersection.
        int res = rayIntersetsTriangle(ray, A, B, C, false, where, depth);
        
        if(res != 0) {
            vec2 U = texelFetch(zdata, ivec2(offset + 3, 0), lod).xy;
            vec2 V = texelFetch(zdata, ivec2(offset + 4, 0), lod).xy;
            vec2 W = texelFetch(zdata, ivec2(offset + 5, 0), lod).xy;
        
            vec2 uv = barycentric(where, A, B, C, U, V, W);
            
            int sampler = int(texelFetch(zdata, ivec2(offset + 3, 0), lod).z);
            vec4 color = texture(textures[sampler], uv);
            
            vec4 blend = vec4(0.0, 0.0, 0.0, 1.0);
            
            
            Photon photon = nearestPhoton(where);
            
            float d = length(photon.position - where);
                
            if(d < 0.3) {
                int bounces = int(photon.meta.z);
            
                if(bounces == 2) {
                    color.b += 1;
                } else if(bounces == 1) {
                    color.g += 1;
                } else {
                    color.r += 1;
                }
                
                // //(0.5-d)*0.7;
            }
            
            /*for(int l = 0; l < numPhotons; ++l) {
                vec3 pos = texelFetch(photons, ivec2(l * 3 + 1, 0), lod).xyz;
                float d = length(pos - where);
                
                if(d < 0.5) {
                    color.r += (0.5-d)*0.7;
                }
            }*/
            
            
            Ray beam;
            beam.place     = where;
            
            
            // For each light
            for(int l = 0; l < lightCount; ++l) {
            
                beam.direction = lightsPosition[l] - beam.place;
                
                int hits = 0;
                
                //if(dot(beam.direction, nurmal) < 0) {
                
                    for(int k = 0; k < numTriangles && hits < 1; ++k) {
                        vec3 tmp;
                        float t;
                        
                        int offset2 = k * stride;
                        vec3 D = texelFetch(zdata, ivec2(offset2 + 0, 0), lod).xyz;
                        vec3 E = texelFetch(zdata, ivec2(offset2 + 1, 0), lod).xyz;
                        vec3 F = texelFetch(zdata, ivec2(offset2 + 2, 0), lod).xyz;
                        
                        // Test the right ray against the current triangle.
                        int res = rayIntersetsTriangle(beam, D, E, F, true, tmp, t);
                        
                        // Test intersection distance.
                        if(res != 0 && (t >= -0.0000001 && t <= 1.0000001)) {
                        
                            vec2 U2 = texelFetch(zdata, ivec2(offset2 + 3, 0), lod).xy;
                            vec2 V2 = texelFetch(zdata, ivec2(offset2 + 4, 0), lod).xy;
                            vec2 W2 = texelFetch(zdata, ivec2(offset2 + 5, 0), lod).xy;
                        
                            vec2 uv = barycentric(tmp, D, E, F, U2, V2, W2);
                            
                            int sampler2 = int(texelFetch(zdata, ivec2(offset2 + 3, 0), lod).z);
                            vec4 color2 = texture(textures[sampler], uv);
                        
                            //if(color2.a > 0.4) {
                                ++hits;
                            //}
                        }
                    }
                //}
                
                const vec4 ambientTerm = vec4(0.2, 0.2, 0.2, 1.0);
                //const vec4 ambientTerm = vec4(0,0,0,0);
            
                // Hit nothing, Full light!
                if(hits < 1) {
                    vec3 n1 = texelFetch(zdata, ivec2(offset + 6, 0), lod).xyz;
                    vec3 n2 = texelFetch(zdata, ivec2(offset + 7, 0), lod).xyz;
                    vec3 n3 = texelFetch(zdata, ivec2(offset + 8, 0), lod).xyz;
                
                    vec3 normal = normalize(barycentric3(where, A, B, C, n1, n2, n3));
                    
                    //vec3 normal = normalize(cross(B - A, C - A));
                    
                    vec3 lightDir = normalize(lightsPosition[l] - where);
                    
                    float lambert = dot(lightDir, normal);// / 10;
                    
                    lambert = max(lambert, 0);
                    
                    blend += lightsDiffuse[l] * lambert + ambientTerm;
                    
                    
                // Hit something, use ambient term
                } else {
                    blend += ambientTerm;
                }
            }
            
            // No alpha channel in light.
            blend.a = 1.0;
            
            blend.r = max(0.1, blend.r);
            blend.g = max(0.1, blend.g);
            blend.b = max(0.1, blend.b);

            zBufferDepth[j] = depth;
            zBufferColor[j] = color;
            
            // There are lights, blend them. Otherwise do nothing.
            if(lightCount > 0) {
                zBufferColor[j] = zBufferColor[j] * blend;
            }
            
            // Move to the next depth slot. TODO: test for limit!
            j++;
        }
    }
    
    vec4 swapV;
    float swapF;
    
    for (int pivot = j; pivot > 1; --pivot) {
        for (int k = 0; k < pivot - 1; ++k) {
            if (zBufferDepth[k] > zBufferDepth[k+1]) {
                swapV = zBufferColor[k];
                zBufferColor[k] = zBufferColor[k+1];
                zBufferColor[k+1] = swapV;
                
                swapF = zBufferDepth[k];
                zBufferDepth[k] = zBufferDepth[k+1];
                zBufferDepth[k+1] = swapF;
            }
        }
    }
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    // Keep blending until there is no alpha or the buffer is empty.
    for(int i = 0; i < j && color.a < 1.0; ++i) {
        color += zBufferColor[i] * zBufferColor[i].a;
    }
    
    return color;
}


void main() {

    // Distance between camera and canvas implies the perspective.
    const float perspective = 4.0;
    finalColor = vec4(0.0, 0.0, 0.0, 1.0);

    finalColor = traceRay(position, perspective);
}