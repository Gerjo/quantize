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
uniform mat4 modelTransform;
uniform mat3 normalTransform;


varying vec4 fragmentColor;
varying vec2 fragmentUV;

varying vec4 tmp;

void main() {
    // Pass data onto fragment shader
    fragmentColor = color;
    fragmentUV    = uv;
    
    tmp = color;
    
    
    vec3 light   = vec3(45.0, 15.0, 10.0);
    
    float intensity = dot(light, normalTransform * normal);

    intensity = clamp(
        intensity,
        0.5,        // Not too dark (minimal light)
        1.6         // Not too light (maximal light)
    );
    
    fragmentColor = vec4(intensity, intensity, intensity, 1);
    
    // Project
    gl_Position = camera * modelTransform * vec4(position, 1);
}
