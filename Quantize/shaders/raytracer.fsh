//
//  raytracer.vsh
//
//  created by Gerard Meier on 19/02/14.
//  copyright (c) 2014 Quantize. All rights reserved.
//


// #pragma optionNV(unroll all)
// #pragma nounroll

uniform vec2 windowSize;        // Size of the viewport
in vec2 position;              // Normalize position on screen


const int MAX_TRIANGLES = 50;

uniform int numTriangles;       // Number of triangles

// An ad-hoc array of structs
uniform vec3 verticesA[MAX_TRIANGLES];  // Triangle vertex #1
uniform vec3 verticesB[MAX_TRIANGLES];  // Triangle vertex #2
uniform vec3 verticesC[MAX_TRIANGLES];  // Triangle vertex #3
uniform vec2 uvA[MAX_TRIANGLES];        // UV vertex edge #1
uniform vec2 uvB[MAX_TRIANGLES];        // UV vertex edge #2
uniform vec2 uvC[MAX_TRIANGLES];        // UV vertex edge #3

// Per triangle, texture sampler index.
uniform int samplers[MAX_TRIANGLES];

// Some transforms
uniform mat4 translation;
uniform mat4 rotation;

// My Intel onboard chip only supports 16 textures. If this becomes a limit,
// we can make an atlas - textures up to 16k resolution are supported.
uniform sampler2D textures[15];

out vec4 finalColor;

struct Ray {
    vec3 place;
    vec3 direction;
};


int rayIntersetsTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2, inout vec3 where, inout float depth) {

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

    // Poor backface culling, this only solves for planes, not "pixels".
    if (t < 0.0) {
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

vec3 transformTriangle(vec3 v) {
    return v + vec3(0, 0, 0);
    //return (rotation * vec4(v, 1.0)).xyz;
}

vec3 transformCamera(vec3 v) {
    //return v;
    return (rotation * vec4(v, 1.0)).xyz;
}

/// Find the barycenter using the weights (UV) and vertices. I spend little time
/// on getting this to work - there might be optimal solutions without so many
/// square roots.
///
///  Read: http://answers.unity3d.com/questions/383804/calculate-uv-coordinates-of-3d-point-on-plane-of-m.html
///  Read: http://en.wikipedia.org/wiki/Barycentric_coordinate_system
///
vec2 barycentric(vec3 f, vec3 v1, vec3 v2, vec3 v3, vec2 uv1, vec2 uv2, vec2 uv3) {

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

    // Color of this "pixel".
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    Ray ray;
    
    // Viewing direction, distance implies the perspective.
    const float perspective = 4.0;
    ray.place = vec3(translation[3][0] * 0.1, 0.0, translation[3][2] * 0.1);
    ray.direction = normalize(vec3(position, ray.place.z + perspective) - ray.place);

    // Rotate the camera
    ray.place = transformCamera(ray.place);
    ray.direction = transformCamera(ray.direction);
    
    // cool idea!
    vec4 zBufferColor[10];
    float zBufferDepth[10];

    // A glsl 4.0 style loop
    int j = 0;
    for(int i = 0; i < numTriangles; ++i) {
        vec3 A = transformTriangle(verticesA[i]);
        vec3 B = transformTriangle(verticesB[i]);
        vec3 C = transformTriangle(verticesC[i]);
    
        vec3 where;
        float depth;
    
        // Ray collision test; "where" is an output: the point of intersection.
        int res = rayIntersetsTriangle(ray, A, B, C, where, depth);

        // TODO: Fancy z-test and alpha blending.
        if(res != 0) {
        
            vec2 uv = barycentric(where, A, B, C, uvA[i], uvB[i], uvC[i]);
        
            //color = texture2D(textures[0], uv);
            
            // Debug colors:
            //color += colors[mod(i, 6)] / 3.0;
            
            zBufferDepth[j] = depth;
            zBufferColor[j++] = colors[mod(i, 6)] / 3.0;
            //zBufferColor[j++] = texture(textures[1], uv);
        }
    }
    
    //color = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 swapV;
    float swapF;
    if (j > 0) {
        while (j > 1) {
            for (int k = 0; k < j - 1; ++k) {
                if (zBufferDepth[k] > zBufferDepth[k+1]) {
                    swapV = zBufferColor[k];
                    zBufferColor[k] = zBufferColor[k+1];
                    zBufferColor[k+1] = swapV;
                    
                    swapF = zBufferDepth[k];
                    zBufferDepth[k] = zBufferDepth[k+1];
                    zBufferDepth[k+1] = swapF;
                }
            }
            j--;
        }
        color = zBufferColor[0];
    }
    else
        color = vec4(0.0, 0.0, 0.0, 0.0);
    
    finalColor = color;//gl_FragColor = color;
}
