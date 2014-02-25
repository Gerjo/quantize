//
//  basic.vsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

in vec3 position;
in vec3 normal;
in vec4 color;
in vec2 uv;
in float samplerIndex;

uniform mat4 camera;
uniform mat4 modelTransform;
uniform mat3 normalTransform;


out vec4 fragmentColor;
out vec2 fragmentUV;
out vec3 fragmentNormal;
out vec3 vertexPosition;
out float fragmentSamplerIndex;

out vec4 tmp;



void main(){
    // The variable needs to be used, else it won't compile.
    tmp = color;
    
    fragmentSamplerIndex = samplerIndex;
    
    vec4 vertPos4 = modelTransform * vec4(position, 1.0);
    
    vertexPosition = vec3(vertPos4) / vertPos4.w;
    fragmentNormal = vec3(normalTransform * normal);
    fragmentUV     = uv;
    fragmentSamplerIndex = samplerIndex;
    
    gl_Position = camera * modelTransform * vec4(position, 1.0);
}


/*
void main() {

    // Pass data onto fragment shader
    fragmentColor = color;
    fragmentUV    = uv;
    
    tmp = color;
    
    
    vec3 light   = vec3(45.0, 15.0, 10.0);
    
    float intensity = dot(light, normalTransform * normal);

    intensity = clamp(
        intensity,
        0.5,        // Not too dark (minimal light)
        1.6         // Not too light (maximal light)
    );
    
    
    fragmentColor = vec4(intensity, intensity, intensity, 1);
    
    // Project
    gl_Position = camera * modelTransform * vec4(position, 1);
}*/
