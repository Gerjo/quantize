//
//  raytracer.fsh
//
//  Created by Gerard Meier on 19/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//



in vec2 vertexPosition;
uniform vec2 windowSize;

uniform mat4 translation;
uniform mat4 rotation;


out vec2 position;


void main() {
    
    position    = vertexPosition;
    
    float aspect = windowSize.x / windowSize.y;
    
    // We only fix the varying for aspect ratio, not the "gl_Position".
    position.x *= aspect;
    
    // Expand to 3rd dimension (because... 3d)
    // Expand to 4th dimension (for depth buffer purposes)
    gl_Position = vec4(vertexPosition, -1.0, 1.0);
}
