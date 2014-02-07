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


void main() {
    // Pass data onto fragment shader
    fragmentColor = color;
    fragmentUV    = uv;
    
    // Project
    gl_Position = camera * transform * vec4(position, 1);
}
