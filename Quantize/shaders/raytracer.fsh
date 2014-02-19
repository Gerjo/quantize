//
//  raytracer.vsh
//
//  Created by Gerard Meier on 19/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//



uniform vec2 windowSize;
varying vec3 position;


void main() {

    vec4 color = vec4(0.0, 0.0, 0.0, 0.5);

    if(position.x > 0.0) {
        color.r = 1.0;
    }

    gl_FragColor = color;
}
