//
//  postp.vsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

attribute vec2 position;
uniform sampler2D texture;

varying vec2 uvmapping;
 
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    
    // Pass on to the fragment shader
    uvmapping = (position + 1.0) / 2.0;
}

