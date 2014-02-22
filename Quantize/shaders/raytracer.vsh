//
//  raytracer.fsh
//
//  Created by Gerard Meier on 19/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//



attribute vec2 vertexPosition;
uniform vec2 windowSize;

uniform mat4 translation;
uniform mat4 rotation;


varying vec2 position;
varying float aspect;


void main() {
    
    
    // Expand to 3D - should make math easier.
    position    = vertexPosition;
    
    //aspect = windowSize.y / windowSize.x;
    
    //position.x *= aspect;
    
    // Expand to 4th dimension (for depth buffer purposes)
    gl_Position = vec4(vertexPosition, -1.0, 1.0);
}
