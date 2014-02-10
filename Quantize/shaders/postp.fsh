//
//  postp.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

uniform sampler2D texture;
varying vec2 uvmapping;
 
void main() {

    const int n = 3;
    //const mat3 kernel = mat3(1.0, 1.0, 1.0,
     //                        1.0, 1.0, 1.0,
     //                        1.0, 1.0, 1.0);
    
    const mat3 kernel = mat3(0.0, 0.0, 0.0,
                             0.0, 1.0, 0.0,
                             0.0, 0.0, 0.0);

    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            
            vec4 sam = texture2D(texture, uvmapping);
            
            color = kernel[i][j] * sam;
            
            //for(int k = 0; k < 3; ++k) {
            //   color[k] = kernel[i][j] * sam[k];
            //}
        }
    }
    
    //color /= float(n * n);

    gl_FragColor = texture2D(texture, uvmapping);
}
