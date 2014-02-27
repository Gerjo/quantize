//
//  raytracer.vsh
//
//  created by Gerard Meier on 19/02/14.
//  copyright (c) 2014 Quantize. All rights reserved.
//


// #pragma optionNV(unroll all)
// #pragma nounroll

uniform int lightCount;
uniform vec3 lightsPosition[10];
uniform vec4 lightsDiffuse[10];
uniform vec4 lightsSpecular[10];
uniform vec4 lightsAmbiant[10];

uniform vec2 windowSize;        // Size of the viewport
in vec2 position;               // Normalize position on screen


uniform int numTriangles;       // Number of triangles


// Some transforms
uniform mat4 translation;
uniform mat4 rotation;


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
const int stride     = 4;        // In floats
const int lod        = 0;        // mipmap level

out vec4 finalColor;

struct Ray {
    vec3 place;
    vec3 direction;
};

vec3 getVertex(int i) {
    int offset = i * stride;
    return texelFetch(zdata, ivec2(offset, 0), lod).xyz;
}

vec2 getUV(int i) {
    int offset = i * stride + 2;
    return texelFetch(zdata, ivec2(offset, 0), lod).xy;
}

int getSampler(int i) {
    int offset = i * stride + 3;
    
    // Retrieve the first float, apply rounding and cast to integer.
    return int(floor(texelFetch(zdata, ivec2(offset, 0), lod).x + 0.5));
}


int rayIntersetsTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2, bool light, inout vec3 where, inout float depth) {

    vec3 a = v1 - v0;
    vec3 b = v2 - v0;
    vec3 n = cross(a, b);
    
    float NdotRaydirection = dot(n, ray.direction);
    
    if(abs(NdotRaydirection) < 0.0001) {
        return 0;
    }
    
    int res = 1;
    
    float dot = dot(n, v0);
    float t = -(dot(n, ray.place) + dot) / NdotRaydirection;

    // Behind the camera culling, only for non light tests.
    if ( !light && t < 0.0) {
        res += 1;
        return 0; // the triangle is behind
    }
    
    // Solve for intersection point. Using this point we determine if it's inside
    // the intersection of each edge.
    where = ray.place + t * ray.direction;
    depth = t;
    
    
    vec3 perp; // vector perpendicular to triangle's plane
    vec3 edge; // Edge direction vector
 
    // Half space tested for edge #0
    edge = v1 - v0;
    perp = cross(edge, where - v0);
    if (dot(n, perp) < 0.0) {
        return 0;
    }
    
    // Half space tested for edge #1
    edge = v2 - v1;
    perp = cross(edge, where - v1);
    if (dot(n, perp) < 0.0) {
        return 0;
    }
    
    // Half space tested for edge #2
    edge = v0 - v2;
    perp = cross(edge, where - v2);
    if (dot(n, perp) < 0.0) {
        return 0;
    }
    
    return res;
}


// Integer modulo.
int mod(int i, int n) {
    return i - i / n * n;
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

/// Find the barycenter using the weights (UV) and vertices. I spend little time
/// on getting this to work - there might be optimal solutions without so many
/// square roots.
///
///  Read: http://answers.unity3d.com/questions/383804/calculate-uv-coordinates-of-3d-point-on-plane-of-m.html
///  Read: http://en.wikipedia.org/wiki/Barycentric_coordinate_system
///
vec2 barycentric(vec3 f, vec3 v1, vec3 v2, vec3 v3, vec2 uv1, vec2 uv2, vec2 uv3) {
    /*
    // Classic Strategy
    // Calculate vectors from point f to vertices v1, v2 and v3:
    vec3 f1 = v1 - f;
    vec3 f2 = v2 - f;
    vec3 f3 = v3 - f;
    
    // Calculate the areas and factors (order of parameters doesn't matter):
    float a  = length(cross(v1 - v2, v1 - v3)); // main triangle area a
    float a1 = length(cross(f2, f3)) / a; // v1's triangle area / a
    float a2 = length(cross(f3, f1)) / a; // v2's triangle area / a
    float a3 = length(cross(f1, f2)) / a; // v3's triangle area / a
    
    // Find the uv corresponding to point f (uv1/uv2/uv3 are associated to v1/v2/v3):
    return uv1 * a1 + uv2 * a2 + uv3 * a3;
     */
    
    /*
    // UVW Strategy
    vec3 uv1p = vec3(uv1, 1.0);
    vec3 uv2p = vec3(uv2, 1.0);
    vec3 uv3p = vec3(uv3, 1.0);
    
    vec3 uvp = uv1p * a1 + uv2p * a2 + uv3p * a3;
    vec3 uv = (uvp / uvp.z);
    return uv.xy;
     */
    
    //Linear System Solver Strategy
    vec3 m0 = v2 - v1;
    vec3 m1 = v3 - v1;
    vec3 m2 = f - v1;
    
    float d00 = dot(m0, m0);
    float d01 = dot(m0, m1);
    float d11 = dot(m1, m1);
    float d20 = dot(m2, m0);
    float d21 = dot(m2, m1);
    float denom = 1 / (d00 * d11 - d01 * d01);
    
    float a = (d11 * d20 - d01 * d21) * denom;
    float b = (d00 * d21 - d01 * d20) * denom;
    float c = 1.0f - a - b;
    
    vec2 uv = uv1 * c + uv2 * a + uv3 * b;
    
    return uv;
}

void main() {

    // Debug colors.
    vec4 colors[6];
    colors[0] = vec4(1, 1, 0, 0.5);
    colors[1] = vec4(0, 1, 0, 0.5);
    colors[2] = vec4(0, 0, 0, 0.5);
    colors[3] = vec4(1, 0, 1, 0.5);
    colors[4] = vec4(0, 1, 0, 0.5);
    colors[5] = vec4(0, 1, 1, 0.5);

    Ray ray;
    
    // Viewing direction, distance implies the perspective.
    const float perspective = 4.0;
    
    // Camera at origin
    ray.place = vec3(0, 0, 0);
    
    // Offset canvas from the camera (substraction is useless, but here for competeness)
    ray.direction = normalize(vec3(position, ray.place.z + perspective) - ray.place);
    
    // Rotate direction about camera
    ray.direction = (rotation * vec4(ray.direction, 1.0)).xyz;
    
    // Translate the camera
    ray.place -= vec3(translation[3][0] * 0.1, translation[3][1] * 0.1, translation[3][2] * 0.1);
    
    // cool idea!
    vec4 zBufferColor[10];
    float zBufferDepth[10];

    
    float infLightCount = 1.0 / lightCount;
    float ambientRatio  = 0.7 * infLightCount;

    int j = 0;
    for(int i = 0; i < numTriangles; ++i) {
        vec3 A = getVertex( i * 3 + 0);
        vec3 B = getVertex( i * 3 + 1);
        vec3 C = getVertex( i * 3 + 2);
        
        vec3 where;
        float depth;
    
        // Ray collision test; "where" is an output: the point of intersection.
        int res = rayIntersetsTriangle(ray, A, B, C, false, where, depth);

        
        if(res != 0) {
            vec2 uv = barycentric(where, A, B, C, getUV(i * 3 + 0), getUV(i * 3 + 1), getUV(i * 3 + 2));
            vec4 color = texture(textures[getSampler(i * 3)], uv);
          
            vec4 blend = vec4(0.0, 0.0, 0.0, 1.0);
            
            // For each light
            for(int l = 0; l < min(lightCount, 2); ++l) {
            //for(int l = 0; l < lightCount; ++l) {
        
               
        
                Ray beam;
                beam.place     = where;
                beam.direction = lightsPosition[l] - beam.place;
                
                int hits = 0;
                
                for(int k = 0; k < numTriangles && hits <= 1; ++k) {
                    vec3 tmp;
                    float t;
                    
                    vec3 D = getVertex( k * 3 + 0);
                    vec3 E = getVertex( k * 3 + 1);
                    vec3 F = getVertex( k * 3 + 2);
                    
                    // Test the right ray against the current triangle.
                    int res = rayIntersetsTriangle(beam, D, E, F, true, tmp, t);
                    
                    // Test intersection distance.
                    if(res != 0 && ( t > 0 || t < 1 )) {
                    
                        vec2 uv2 = barycentric(where, D, E, F, getUV(k * 3 + 0), getUV(k * 3 + 1), getUV(k * 3 + 2));
                        vec4 color2 = texture(textures[getSampler(k * 3)], uv);
                    
                        if(color2.a != 0.0) {
                            ++hits;
                        }
                    }
                }
                
                // Hit nothing, Full light!
                if(hits <= 1) {
                    blend += lightsDiffuse[l] * infLightCount;
                    
                // Hit something, use ambient term
                } else {
                    blend += vec4(0.2, 0.2, 0.2, 1.0);//lightsDiffuse[l] * ambientRatio;
                }
                
            }
            
            // No alpha channel in light.
            blend.a = 1.0;
            
            zBufferDepth[j] = depth;
            //zBufferColor[j] = colors[mod(i, 6)] / 3.0;
            zBufferColor[j] = color * blend;
            
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
    
    // Final of this "pixel".
    finalColor = vec4(0.0, 0.0, 0.0, 0.0);
    
    // Keep blending until there is no alpha or the buffer is empty.
    for(int i = 0; i < j && finalColor.a < 1.0; ++i) {
        finalColor += zBufferColor[i] * zBufferColor[i].a;
    }
    
}
