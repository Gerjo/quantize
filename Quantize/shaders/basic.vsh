//
//  Shader.vsh
//  derp
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

attribute vec3 position;
attribute vec3 normal;
attribute vec4 color;

uniform mat4 camera;
uniform mat4 transform;

varying vec4 fragmentColor;


void main() {
    fragmentColor = color;
    
    gl_Position = camera * vec4(position, 1);// * transform;
}
