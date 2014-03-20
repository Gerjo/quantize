//
//  photon.fsh
//
//  Created by Gerard Meier on 18/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "shaders/common.glsl"

in vec2 uvmapping;
in vec2 uvunit;

out vec4 finalColor;

uniform int lightCount;          // Amount of light sources
uniform vec3 lightPositions[10];  // Position of each light, up to 10.

void main() {
    finalColor = vec4(1, 0, 0, 0.5);
}
