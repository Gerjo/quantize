//
//  postp.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

//------------------------------------------------------------------------------------------

/*
out vec4 finalColor;


void main() {
    finalColor = vec4(0.0, 0.0, 0.0, 0.0);
 }
*/


//------------------------------------------------------------------------------------------



in vec2 uvmapping;
in vec2 uvunit;

uniform sampler2D uniformTexture;
uniform int kernelType;
uniform float kernelLerp;


out vec4 finalColor;


// 0
const mat3 identity = mat3( 0.0,  0.0,  0.0,
                            0.0,  1.0,  0.0,
                            0.0,  0.0,  0.0);
// 1
const mat3 blur1    = mat3( 1.0,  1.0,  1.0,
                            1.0,  1.0,  1.0,
                            1.0,  1.0,  1.0);
// 2
const mat3 blur2    = mat3( 1.0,  2.0,  1.0,
                            2.0,  4.0,  2.0,
                            1.0,  2.0,  1.0);
// 3
const mat3 edge1    = mat3( 0.0,  1.0,  0.0,
                            1.0, -4.0,  1.0,
                            0.0,  1.0,  0.0);
// 4
const mat3 edge2    = mat3( 1.0,  0.0, -1.0,
                            0.0,  0.0,  0.0,
                           -1.0,  0.0,  1.0);
// 5
const mat3 edge3    = mat3(-1.0, -1.0, -1.0,
                           -1.0,  8.0, -1.0,
                           -1.0, -1.0, -1.0);
// 6
const mat3 sharpen  = mat3( 0.0, -1.0,  0.0,
                           -1.0,  5.0, -1.0,
                            0.0, -1.0,  0.0);


void main() {

    
    mat3 kernel = identity;
    
    if(kernelType == 1)
        kernel = blur1;
    if(kernelType == 2)
        kernel = blur2;
    if(kernelType == 3)
        kernel = edge1;
    if(kernelType == 4)
        kernel = edge2;
    if(kernelType == 5)
        kernel = edge3;
    if(kernelType == 6)
        kernel = sharpen;
 
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for(int x = 0; x < 3; ++x) {
        for(int y = 0; y < 3; ++y) {
            
            vec4 sam = texture(uniformTexture, uvmapping + vec2(x-1, y-1) * uvunit);
            
            color += sam * kernel[x][y];
        }
    }
    
    
    if(kernelType == 1 || kernelType == 2) {
        // Normalize term for blur shading
        color /= 9.0;
    }
    
    // Restore the alpha channel.
    color.a = texture(uniformTexture, uvmapping).a;
 
    //
    finalColor = mix(color, texture(uniformTexture, uvmapping), kernelLerp);
}
