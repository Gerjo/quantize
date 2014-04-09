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

uniform int maxBounces;         // Amount of bounce iterations

uniform vec2 windowSize;        // Size of the viewport
in vec2 position;               // Normalize position on screen
in vec2 pixelPosition;          // The approximate pixel on screen.

uniform int numTriangles;       // Number of triangles

uniform int useLambertian;
uniform int showPhotons;

// Some transforms
uniform mat4 translation;
uniform mat4 rotation;

// Used for randomness
uniform int frameCounter;
uniform float time;

uniform ivec3 gridResolution;
uniform vec3  gridMin;
uniform vec3  gridMax;
uniform vec3  gridInterval;

uniform float totalFlux;       // Total flux in the world

// My Intel onboard chip only supports 16 textures. If this becomes a limit,
// we can make an atlas - textures up to 16k resolution are supported. Reading
// the spec further, we can use a sampler2DArray!
uniform sampler2D textures[13];


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

uniform sampler2D gridTexture;

struct Photon {
    vec3 direction;
    vec3 position;
    vec3 meta;          // dead, color and bounces
};

vec4 computeDirectLight(in int texelOffset, in vec3 where, in vec3 A, in vec3 B, in vec3 C) {

    vec4 direct = vec4(0, 0, 0, 0);

    Ray beam;
    beam.place     = where;
     
     
    // Test each light against the world
    for(int l = 0; l < lightCount; ++l) {
     
        beam.direction = lightsPosition[l] - beam.place;
        
        bool hitSomething = false;
        
        for(int k = 0; k < numTriangles; ++k) {
            vec3 tmp;
            float t;
            
            // Find the vertices of this triangle
            int texelOffset = k * stride;
            vec3 D = texelFetch(zdata, ivec2(texelOffset + 0, 0), lod).xyz;
            vec3 E = texelFetch(zdata, ivec2(texelOffset + 1, 0), lod).xyz;
            vec3 F = texelFetch(zdata, ivec2(texelOffset + 2, 0), lod).xyz;
             
            // Test the right ray against the current triangle.
            bool collision = rayIntersetsTriangle(beam, D, E, F, true, tmp, t);
             
            // Test intersection distance.
            if( ! collision && (t >= -0.0000001 && t <= 1.0000001)) {
             
                // Use this to let direct light pass through semi transparent
                // objects.
                //vec2 U2 = texelFetch(zdata, ivec2(texelOffset + 3, 0), lod).xy;
                //vec2 V2 = texelFetch(zdata, ivec2(texelOffset + 4, 0), lod).xy;
                //vec2 W2 = texelFetch(zdata, ivec2(texelOffset + 5, 0), lod).xy;
             
                //vec2 uv = barycentric(tmp, D, E, F, U2, V2, W2);
                 
                //int sampler2 = int(texelFetch(zdata, ivec2(texelOffset + 3, 0), lod).z);
                //vec4 color2 = texture(textures[sampler2], uv);
             
                hitSomething = true;
                break;
            }
        }
         
        const vec4 ambientTerm = vec4(0.2, 0.2, 0.2, 1.0);
     
        // Did not hit anything. Apply light.
        if( ! hitSomething) {
            vec3 n1 = texelFetch(zdata, ivec2(texelOffset + 6, 0), lod).xyz;
            vec3 n2 = texelFetch(zdata, ivec2(texelOffset + 7, 0), lod).xyz;
            vec3 n3 = texelFetch(zdata, ivec2(texelOffset + 8, 0), lod).xyz;
         
            vec3 normal   = normalize(barycentric3(where, A, B, C, n1, n2, n3));
            vec3 lpos     = lightsPosition[l];
            vec3 lightDir = normalize(lpos - where);
            float lambert = max(dot(lightDir, normal), 0);
             
            direct += lightsDiffuse[l] * lambert;
        }
    }
    
    return direct;
}


vec4 computeGlobalIllumination(in vec3 where) {
  
    // Light term added by photons
    vec4 photonIntensity = vec4(0, 0, 0, 0);
    
    // Quantize pixel location to a grid location
    ivec3 gridQuantize = ivec3(
        round((where.x - gridMin.x) / gridInterval.x),
        round((where.y - gridMin.y) / gridInterval.y),
        round((where.z - gridMin.z) / gridInterval.z)
    );
    
    float flux      = 0.0;
    int photonCount = 0;
    
    const int shiftRange = 1;
    
    for (int xShift = -shiftRange; xShift < shiftRange; xShift++) {
        for (int yShift = -shiftRange; yShift < shiftRange; yShift++) {
            for (int zShift = -shiftRange; zShift < shiftRange; zShift++) {
                ivec3 gridShifted = ivec3(
                    gridQuantize.x + xShift,
                    gridQuantize.y + yShift,
                    gridQuantize.z + zShift
                );
                
                if (gridShifted.x < 0 || gridShifted.y < 0 || gridShifted.z < 0) {
                    continue;
                }
                
                if (gridShifted.x >= gridResolution.x || gridShifted.y >= gridResolution.y || gridShifted.z >= gridResolution.z) {
                    continue;
                }
                
                int cellIndex = gridShifted.z + (gridShifted.y * gridResolution.x)
                                + (gridShifted.x * gridResolution.x * gridResolution.y);
                
                int textureWidth = textureSize(gridTexture, lod).x;
                
                ivec2 texIndex = indexWrap(cellIndex, textureWidth);
                vec3 cell      = texelFetch(gridTexture, texIndex, lod).xyz;
                
                const int photonStride = 3; // 3 x vec3 [direction, position, meta]
                photonCount    += int(cell.x);
                int startIndex  = int(cell.y);
                int endIndex    = int(cell.z);
                
                
                for(int i = startIndex, j = startIndex, max = 50; max != 0 && i < endIndex; --max, ++i, j += photonStride) {
                    vec3 photonDirection = texelFetch(gridTexture, indexWrap(j + 0, textureWidth), lod).xyz;
                    vec3 photonPosition  = texelFetch(gridTexture, indexWrap(j + 1, textureWidth), lod).xyz;
                    vec3 photonMeta      = texelFetch(gridTexture, indexWrap(j + 2, textureWidth), lod).xyz; // dead color bounces
                    
                    float d = length(photonPosition - where);
                    
                    if(int(photonPosition.x) == 2) {
                        //photonIntensity.r = 3;
                    }
                    
                    //if(d < 0.4) {
                    //flux += 0.1 / d;
                    //}
                    
                    //d = 0.1 / d;
                    //d = -pow(d, 2) + 2;
                    d = 1.0 * exp(-(pow(d,2) / 1.2));
                    
                    if (d > 0.0) {
                        flux += photonMeta.z * d;//pow(0.1, maxBounces - photonMeta.z) * d;
                    }
                }
            }
        }
    }
    

    flux = 1.0 * log(flux * 0.5);

    // No photons? Must be an error surface. Make it green!
    if(photonCount == 0) {
        photonIntensity.g = 3;
        
    } else {
        // Photons have no color yet.
        photonIntensity.x = flux / 2;
        photonIntensity.y = photonIntensity.x;
        photonIntensity.z = photonIntensity.x;
    }

    return photonIntensity;
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
    
    int currentBuffer = 0;
    for(int i = 0; i < numTriangles; ++i) {
        
        int texelOffset = i * stride;
        vec3 A = texelFetch(zdata, ivec2(texelOffset + 0, 0), lod).xyz;
        vec3 B = texelFetch(zdata, ivec2(texelOffset + 1, 0), lod).xyz;
        vec3 C = texelFetch(zdata, ivec2(texelOffset + 2, 0), lod).xyz;
        
        vec3 where;
        float depth;
        
        // Ray collision test; "where" is an output: the point of intersection.
        bool collision = rayIntersetsTriangle(ray, A, B, C, false, where, depth);
        
        if( ! collision) {
            vec2 U  = texelFetch(zdata, ivec2(texelOffset + 3, 0), lod).xy;
            vec2 V  = texelFetch(zdata, ivec2(texelOffset + 4, 0), lod).xy;
            vec2 W  = texelFetch(zdata, ivec2(texelOffset + 5, 0), lod).xy;
            vec2 uv = barycentric(where, A, B, C, U, V, W);
            
            int sampler = int(texelFetch(zdata, ivec2(texelOffset + 3, 0), lod).z);
            
            // Color of the texture
            vec4 color = texture(textures[sampler], uv);
            
            vec4 blend = vec4(0.0, 0.0, 0.0, 1.0);
            
            vec4 direct = computeDirectLight(texelOffset, where, A, B, C);
            vec4 global = computeGlobalIllumination(where);
            
            blend += direct;
            blend += global;

            
            // No alpha channel in light.
            blend.a = 1.0;
            
            // Cap the light blending term
            blend.r = max(0.1, blend.r);
            blend.g = max(0.1, blend.g);
            blend.b = max(0.1, blend.b);

            zBufferDepth[currentBuffer] = depth;
            zBufferColor[currentBuffer] = color;
            
            // There are lights, blend them. Otherwise do nothing.
            if(lightCount > 0) {
                zBufferColor[currentBuffer] = zBufferColor[currentBuffer] * blend;
            }
            
            // Move to the next depth slot.
            if(++currentBuffer > maxBuffer) {
                break;
            }
        }
    }
    
    
    vec4 swapV;
    float swapF;
    
    
    // Sort depth buffer
    for (int pivot = currentBuffer; pivot > 1; --pivot) {
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
    for(int i = 0; i < currentBuffer && color.a < 1.0; ++i) {
        color += zBufferColor[i] * zBufferColor[i].a;
    }
    
    return color;
}


void main() {

    // Distance between camera and canvas implies the perspective.
    const float perspective = 4.0;

    finalColor = traceRay(position, perspective);
}