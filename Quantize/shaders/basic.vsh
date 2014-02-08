//
//  basic.vsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

attribute vec3 position;
attribute vec3 normal;
attribute vec4 color;
attribute vec2 uv;

uniform mat4 camera;
uniform mat4 transform;

varying vec4 fragmentColor;
varying vec2 fragmentUV;

varying vec4 tmp;

void main() {
    // Pass data onto fragment shader
    fragmentColor = color;
    fragmentUV    = uv;
    
    tmp = color;
    
    vec4 light = vec4(10.0, 10.0, 10.0, 1.0);
    vec4 normal4 = camera * transform * vec4(normal, 1.0);
    
    float intensity = dot(light, normal4);
    
    intensity = min(1.6, intensity);
    intensity = max(0.5, intensity);
    
    fragmentColor = vec4(intensity, intensity, intensity, 1);
    
    // Project
    gl_Position = camera * transform * vec4(position, 1);
}
