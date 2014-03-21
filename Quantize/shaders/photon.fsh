//
//  photon.fsh
//
//  Created by Gerard Meier on 18/03/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "shaders/common.glsl"

in vec2 uvmapping;
in vec2 uvunit;

// Previous bounce
uniform vec4 uniformColor;
uniform vec4 uniformPosition;
uniform vec4 uniformMeta;

// New bounce
out vec4 outColor;
out vec4 outPosition;
out vec4 outMeta;

uniform int lightCount;           // Amount of light sources
uniform vec3 lightPositions[10];  // Position of each light, up to 10.

void main() {
    outColor    = vec4(1, 0, 0, 0.5);
    outPosition = vec4(0, 0, 1, 0.5);
    outMeta     = vec4(0, 1, 0, 0.5);
}
