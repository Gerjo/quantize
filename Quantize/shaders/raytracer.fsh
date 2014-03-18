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


// My Intel onboard chip only supports 16 textures. If this becomes a limit,
// we can make an atlas - textures up to 16k resolution are supported. Reading
// the spect further, we can use a sampler2DArray!
uniform sampler2D textures[15];


/// Format (non optimized structure, directly copied from c++)
/// [0] Vector3 position;
/// [1] Vector3 normal;
/// [2] Vector2 uv;
/// [2] unsigned char color[4];
/// [3] GLuint sampler;
/// [3] Glbyte[2] padding;
uniform sampler2D zdata;

const float Infinity = 99999999; // Infinity, for all intents and purposes.
const int stride     = 6;        // In vec3
const int lod        = 0;        // mipmap level

out vec4 finalColor;






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
    
    const int maxBuffer = 3;
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
            
            /*if(useTexture == 0) {
                float size = 1;
                int foo = int(floor(where.x / 1) * 1);
                int bar = int(floor(where.z / 1) * 1);
                
                if(mod(foo + bar, 2) == 0) {
                    color = vec4(1.0, 1.0, 1.0, 1.0);
                } else {
                    color = vec4(0.0, 0.0, 0.0, 1.0);
                }
            }*/
            
            vec4 blend = vec4(0.0, 0.0, 0.0, 1.0);
            
            Ray beam;
            beam.place     = where;
            
            // For each light
            for(int l = 0; l < lightCount; ++l) {
                
                beam.direction = lightsPosition[l] - beam.place;
                
                int hits = 0;
                
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
                    
                        if(color2.a > 0.4) {
                            ++hits;
                        }
                    }
                }
            
                // Hit nothing, Full light!
                if(hits < 1) {
                    blend += lightsDiffuse[l] * infLightCount;
                    
                // Hit something, use ambient term
                } else {
                    blend += vec4(0.2, 0.2, 0.2, 1.0);//lightsDiffuse[l] * ambientRatio;
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

    //if(mod(int(floor((pixelPosition.x / 2 + 1) * windowSize.x + 0.5)), 2) == 0) {
    
    int row = int(floor((pixelPosition.x / 2 + 1) * windowSize.x));
    int col = int(floor((pixelPosition.y / 2 + 1) * windowSize.y));
    
    //if(mod(col, 5) != mod(frameCounter, 5)) {
    //    finalColor = vec4(0, 0, 0, 0);
    //    return;
    //}

//#define RANDOM
#define NONE
//#define STRATIFICATION

    
    // Distance between camera and canvas implies the perspective.
    const float perspective = 4.0;
    finalColor = vec4(0.0, 0.0, 0.0, 0.0);
    
#ifdef NONE

    finalColor = traceRay(position, perspective);

#endif
    
#ifdef RANDOM
    int iterations = n;
    float deviationX = range / windowSize.x;
    float deviationY = range / windowSize.y;
    vec2 randomPos;
    vec2 randomSeed = position;
    float randomDX;
    float randomDY;
    vec4 itColor = vec4(0.0, 0.0, 0.0, 0.0);
    
    float derpma = sigma; // Is a global uniform
    float sigmaPrecomputed = 1.0 / (2.0 * derpma * derpma);
    float sigmaSum = 0.0;

    for (int i = 0; i < iterations; ++i) {
        //calculate randomised deviation
        randomPos = position;
        
        randomDX = fract(sin(dot(randomSeed.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5;
        randomDY = fract(cos(dot(randomSeed.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5;
        
        randomSeed.x += randomSeed.y + 0.27182;
        randomSeed.y += randomSeed.x;
        
        float deltaX = randomDX * deviationX;
        float deltaY = randomDY * deviationY;
        
        randomPos.x += deltaX;
        randomPos.y += deltaY;
        
        deltaX *= 100.0;
        deltaY *= 100.0;
        
        //raytracer go!
        itColor = traceRay(randomPos, perspective);
        
        
        if(sigma != 0.0) {
            // Linear
            //float w = 1 / (sqrt(deltaX * deltaX + deltaY * deltaY));
        
            // Bell curve
            float w = exp(-((deltaX * deltaX + deltaY * deltaY) * sigmaPrecomputed));
        
            // Accumulate sigma for normalisation
            sigmaSum += w;
       
            //finalColor.a = 1.0;
            finalColor += itColor * w;
        } else {
            //add to average
            finalColor += (itColor / iterations);
        }
    }
    
    if(sigma != 0.0) {
        finalColor /= sigmaSum;
    }
    
#endif //RANDOM
    
#ifdef STRATIFICATION
    //Stratification degree. Set to 0 to disable.
    int stratDegree = n;
    float stratIterations = pow(2 * stratDegree + 1, 2);
    int stratDivisions = 2 * (stratDegree + 1);
    float stratIntervalX = 2.0 / (float(stratDivisions) * windowSize.x);
    float stratIntervalY = 2.0 / (float(stratDivisions) * windowSize.y);
    vec2 stratPos;
    vec2 randomSeed = position;
    float randomDX;
    float randomDY;
    vec4 stratTemp;
    for (int stratX = 0 - stratDegree; stratX < 1 + stratDegree; ++stratX) {
        for (int stratY = 0 - stratDegree; stratY < 1 + stratDegree; ++stratY) {
            //calculate randomised deviation
            stratPos = position;
            
            randomDX = fract(sin(dot(randomSeed.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5;
            randomDY = fract(cos(dot(randomSeed.xy, vec2(78.233, 12.9898))) * 43758.5453) - 0.5;
            
            randomSeed.x += randomSeed.y + 0.014159268;
            randomSeed.y += randomSeed.x;
            
            if(enableJitter == 1) {
                stratPos.x += float(stratX + randomDX) * stratIntervalX;
                stratPos.y += float(stratY + randomDY) * stratIntervalY;
            }
            
            stratPos.x += float(stratX) * stratIntervalX;
            stratPos.y += float(stratY) * stratIntervalY;
            
            
            //raytracer go!
            stratTemp = traceRay(stratPos, perspective);
            
            //add to average
            finalColor += (stratTemp / stratIterations);
        }
    }
#endif //STRATIFICATION
}