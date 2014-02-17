//
//  postp.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

uniform sampler2D texture;

varying vec2 uvmapping;
varying vec2 uvunit;


const mat3 identity = mat3( 0.0,  0.0,  0.0,
                            0.0,  1.0,  0.0,
                            0.0,  0.0,  0.0);

const mat3 blur1    = mat3( 1.0,  1.0,  1.0,
                            1.0,  1.0,  1.0,
                            1.0,  1.0,  1.0);

const mat3 blur2    = mat3( 1.0,  2.0,  1.0,
                            2.0,  4.0,  2.0,
                            1.0,  2.0,  1.0);

const mat3 edge1    = mat3( 0.0,  1.0,  0.0,
                            1.0, -4.0,  1.0,
                            0.0,  1.0,  0.0);

const mat3 edge2    = mat3( 1.0,  0.0, -1.0,
                            0.0,  0.0,  0.0,
                           -1.0,  0.0,  1.0);

const mat3 edge3    = mat3(-1.0, -1.0, -1.0,
                           -1.0,  8.0, -1.0,
                           -1.0, -1.0, -1.0);

const mat3 sharpen  = mat3( 0.0, -1.0,  0.0,
                           -1.0,  5.0, -1.0,
                            0.0, -1.0,  0.0);


void main() {

    
    const mat3 kernel = sharpen;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for(int x = 0; x < 3; ++x) {
        for(int y = 0; y < 3; ++y) {
            
            vec4 sam = texture2D(texture, uvmapping + vec2(x-1, y-1) * uvunit);
            
            color += sam * kernel[x][y];
        }
    }
    
    
    
    // Normalize term for blur shading
    //color /= 9.0;
    
    // Restore the alpha channel.
    color.a = texture2D(texture, uvmapping).a;
    
    //
    gl_FragColor = mix(color, texture2D(texture, uvmapping), 0.7);
}
