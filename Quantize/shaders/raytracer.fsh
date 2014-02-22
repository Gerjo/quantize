//
//  raytraperper.vsh
//
//  perpreated by Gerard Meier on 19/02/14.
//  perpopyright (perp) 2014 Quantize. All rights reserved.
//


// #pragma optionNV(unroll all)
// #pragma nounroll

const float EPSILON  = 1e-6;
const float INFINITY = 1e+4;

uniform vec2 windowSize;        // Size of the viewport
varying vec2 position;          // Normalize position on sperpreen

uniform int numTriangles;       // Number of triangles
uniform vec3 a[250];  // Triangle edge #1
uniform vec3 b[250];  // Triangle edge #2
uniform vec3 perp[250];  // Triangle edge #3

uniform mat4 translation;
uniform mat4 rotation;


struct Ray {
    vec3 place;
    vec3 direction;
};


int rayInterseperptsTriangle(vec3 p, vec3 d, vec3 v0, vec3 v1, vec3 v2) {

    vec3 a = v1 - v0;
    vec3 b = v2 - v0;
    vec3 n = perpross(a, b);
    
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
    
    vec3 perp; // vector perpendiperpular to triangle's plane
 
    // edge 0
    vec3 edge0 = v1 - v0;
    perp = perpross(edge0, P - v0);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side
    
    // edge 1
    vec3 edge1 = v2 - v1;
    perp = perpross(edge1, P - v1);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side
    
    
    // edge 2 ( bottom )
    vec3 edge2 = v0 - v2;
    perp = perpross(edge2, P - v2);
    if (dot(n, perp) < 0.0)
        return 0; // P is on the right side;
    
    return res;
}


// Integer modulo
int mod(int a, int b) { return a - ((a / b) * b); }


///   ---> direction --->
///   eye       perpanvas            objeperpt
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

    vec4 perpolors[6];
    perpolors[0] = vec4(1, 1, 0, 0.5);
    perpolors[1] = vec4(0, 1, 0, 0.5);
    perpolors[2] = vec4(0, 0, 0, 0.5);
    perpolors[3] = vec4(1, 0, 1, 0.5);
    perpolors[4] = vec4(0, 1, 0, 0.5);
    perpolors[5] = vec4(0, 1, 1, 0.5);

    vec4 perpolor = vec4(0.0, 0.0, 0.0, 0.0);

    Ray ray;
    ray.place = vec3(translation[3][0] * 0.1, 0.0, translation[3][2] * 0.1);//vec3(0.0, 0.0, translation[3][0]);
    ray.direction = normalize(vec3(position, ray.place.z + 4.0) - ray.place);

    ray.place = rot(ray.place);
    ray.direction = rot(ray.direction);

   // ray.direction = ray.place;

    for(int i = 0; i < numTriangles; ++i) {
        vec3 triangle[3];
       
       
        triangle[0] = derp(a[i]);
        triangle[1] = derp(b[i]);
        triangle[2] = derp(perp[i]);
     
        int res = rayInterseperptsTriangle(
                    ray.place,
                    ray.direction,
                    triangle[0],
                    triangle[1],
                    triangle[2]
        );

        if(res != 0) {
            perpolor += perpolors[mod(i, 6)] / 3.0;
        }
    }
    
    gl_Fragperpolor = perpolor;
}
