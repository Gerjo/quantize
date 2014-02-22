//
//  raytracer.vsh
//
//  created by Gerard Meier on 19/02/14.
//  copyright (c) 2014 Quantize. All rights reserved.
//


// #pragma optionNV(unroll all)
// #pragma nounroll

const float EPSILON  = 1e-6;
const float INFINITY = 1e+4;

uniform vec2 windowSize;        // Size of the viewport
varying vec2 position;          // Normalize position on screen

uniform int numTriangles;       // Number of triangles
uniform vec3 a[250];  // Triangle edge #1
uniform vec3 b[250];  // Triangle edge #2
uniform vec3 c[250];  // Triangle edge #3

uniform mat4 translation;
uniform mat4 rotation;


struct Ray {
    vec3 place;
    vec3 direction;
};


int rayIntersetsTriangle(vec3 p, vec3 d, vec3 v0, vec3 v1, vec3 v2) {

    vec3 a = v1 - v0;
    vec3 b = v2 - v0;
    vec3 n = cross(a, b);
    
    float NdotRaydirection = dot(n, d);
    
    if(abs(NdotRaydirection) < 0.01) {
        return 0;
    }
    
    int res = 1;
    
    float dot = dot(n, v0);
    float t = -(dot(n, p) + dot) / NdotRaydirection;

    if (t < 0.0) {
        res += 1;
        //return 0; // the triangle is behind
    }
    
    vec3 P = p + t * d;
    
    vec3 perp; // vector perpendicular to triangle's plane
 
    // edge 0
    vec3 edge0 = v1 - v0;
    perp = cross(edge0, P - v0);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side
    
    // edge 1
    vec3 edge1 = v2 - v1;
    perp = cross(edge1, P - v1);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side
    
    
    // edge 2 ( bottom )
    vec3 edge2 = v0 - v2;
    perp = cross(edge2, P - v2);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side;
    
    return res;
}


// Integer modulo
int mod(int a, int b) { return a - ((a / b) * b); }


///   ---> direction --->
///   eye       canvas            object
///       /      |
///      /       |                 +------+
///     /        |                 |      |
///   #          |                 |      |
///     \        |                 +------+
///      \       |
///       \      |

vec3 derp(vec3 v) {
    return v + vec3(0, 0, 0);
    //return (rotation * vec4(v, 1.0)).xyz;
}

vec3 rot(vec3 v) {
    //return v;
    return (rotation * vec4(v, 1.0)).xyz;
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
    ray.place = vec3(translation[3][0] * 0.1, 0.0, translation[3][2] * 0.1);//vec3(0.0, 0.0, translation[3][0]);
    ray.direction = normalize(vec3(position, ray.place.z + perspective) - ray.place);

    // Rotate the camera
    ray.place = rot(ray.place);
    ray.direction = rot(ray.direction);
    
    // cool idea!
    vec4 zBuffer[10];

    // A glsl 4.0 style loop
    for(int i = 0, j = 0; i < numTriangles; ++i) {
    
        // Ray collision test
        int res = rayIntersetsTriangle(
                    ray.place,
                    ray.direction,
                    derp(a[i]),
                    derp(b[i]),
                    derp(c[i])
        );

        // TODO: Fancy z-test and alpha blending.
        if(res != 0) {
            color += colors[mod(i, 6)] / 3.0;
            
            //zBuffer[j] = somecolor;
        }
    }
    
    gl_FragColor = color;
}
