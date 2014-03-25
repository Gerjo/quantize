//
//  photon.vsh
//
//  Created by Gerard Meier on 18/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

in vec2 position;

uniform vec2 windowSize;

out vec2 uvmapping;
out vec2 uvunit;

out vec2 localPosition;
out vec2 pixelPosition;

void main() {
    
    localPosition = position;
    pixelPosition = position;

    float aspect = windowSize.x / windowSize.y;
    
    // Scale GL coordinates to screen.
    localPosition.y /= aspect;
    
    
    // Pass on to the fragment shader
    uvmapping = (position + 1.0) / 2.0;
    uvunit    = 1.0 / windowSize;
    
    // gl_Position = vec4(0.0,0.0,0.0,1.0);
    gl_Position = vec4(position, 0.0, 1.0);
}
