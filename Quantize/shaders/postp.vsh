//
//  postp.vsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//



in vec2 position;

uniform vec2 windowSize;

out vec2 uvmapping;
out vec2 uvunit;

void main() {
    
    // Pass on to the fragment shader
    uvmapping = (position + 1.0) / 2.0;
    uvunit    = 1.0 / windowSize;
    
    // gl_Position = vec4(0.0,0.0,0.0,1.0);
    gl_Position = vec4(position, 0.0, 1.0);
}
