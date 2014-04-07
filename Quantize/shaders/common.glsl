//
//  common.glsl
//
//  Created by Gerard Meier on 19/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

const float Infinity = 1. / 0.;

struct Ray {
    vec3 place;
    vec3 direction;
};

/// Integer modulo.
int mod(int i, int n) {
    return i - i / n * n;
}

/// Number rounding
int round(float f) {
    return int(floor(f + 0.5));
}

float random(in vec2 seed) {
    return fract(cos(dot(seed, vec2(78.233, 12.9898))) * 43758.5453) - 0.5;
}

float random(in vec3 seed) {
    return random(seed.xy);
}

float random(in float x, in float y) {
    return random(vec2(x, y));
}




// Some initial seed.
int _seed = 10;

void srand(float s) {
    _seed = int(s * 12345);
}

float rand() {
    _seed = (1103515245 * _seed + 12345) % 2147483648;

    return fract(_seed * 0.001);
}

/// Find the barycenter using the weights (UV) and vertices. I spend little time
/// on getting this to work - there might be optimal solutions without so many
/// square roots.
///
///  Read: http://answers.unity3d.com/questions/383804/calculate-uv-coordinates-of-3d-point-on-plane-of-m.html
///  Read: http://en.wikipedia.org/wiki/Barycentric_coordinate_system
///
vec2 barycentric(in vec3 f, in vec3 v1, in vec3 v2, in vec3 v3, in vec2 uv1, in vec2 uv2, in vec2 uv3) {
    //Linear System Solver Strategy
    vec3 m0 = v2 - v1;
    vec3 m1 = v3 - v1;
    
    
    float d00 = dot(m0, m0);
    float d01 = dot(m0, m1);
    float d11 = dot(m1, m1);
    float denom = 1 / (d00 * d11 - d01 * d01);
    
    vec3 m2   = f - v1;
    float d20 = dot(m2, m0);
    float d21 = dot(m2, m1);
    
    float a = (d11 * d20 - d01 * d21) * denom;
    float b = (d00 * d21 - d01 * d20) * denom;
    float c = 1.0f - a - b;
    
    vec2 uv = uv1 * c + uv2 * a + uv3 * b;
    
    return uv;
}


vec3 barycentric3(in vec3 f, in vec3 v1, in vec3 v2, in vec3 v3, in vec3 uv1, in vec3 uv2, in vec3 uv3) {
    //Linear System Solver Strategy
    vec3 m0 = v2 - v1;
    vec3 m1 = v3 - v1;
    
    
    float d00 = dot(m0, m0);
    float d01 = dot(m0, m1);
    float d11 = dot(m1, m1);
    float denom = 1 / (d00 * d11 - d01 * d01);
    
    vec3 m2   = f - v1;
    float d20 = dot(m2, m0);
    float d21 = dot(m2, m1);
    
    float a = (d11 * d20 - d01 * d21) * denom;
    float b = (d00 * d21 - d01 * d20) * denom;
    float c = 1.0f - a - b;
    
    vec3 uv = uv1 * c + uv2 * a + uv3 * b;
    
    return uv;
}


// Source: http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
int rayIntersetsTriangle(in Ray ray, in vec3 v0, in vec3 v1, in vec3 v2, in bool light, out vec3 where, out float depth) {

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    
    vec3 h  = cross(ray.direction, e2);
	float a = dot(e1, h);

	if (a > -0.00001 && a < 0.00001) {
		return 0;
    }
    
	float f = 1 / a;
	vec3 s  = ray.place - v0;
	float u = f * dot(s, h);

	if (u < 0.0 || u > 1.0) {
		return 0;
    }
	
    vec3 q  = cross(s, e1);
	float v = f * dot(ray.direction, q);

	if (v < 0.0 || u + v > 1.0) {
		return 0;
    }
    
	// at this stage we can compute t to find out where
	// the intersection point is on the line
	depth = f * dot(e2, q);
    
	if (depth > 0.00001) {// ray intersection
        where = ray.place + depth * ray.direction;

        return 1;
    } else { // this means that there is a line intersection
		 // but not a ray intersection
		 return 0;
    }
}
