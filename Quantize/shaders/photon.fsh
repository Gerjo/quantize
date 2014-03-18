//
//  photon.fsh
//
//  Created by Gerard Meier on 18/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "shaders/common.glsl"

in vec2 uvmapping;
in vec2 uvunit;

//uniform sampler2D uniformTexture;


out vec4 finalColor;


void main() {
    finalColor = vec4(1, 0, 0, 1);//texture(uniformTexture, uvmapping);
}
