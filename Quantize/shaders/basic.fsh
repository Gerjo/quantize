//
//  basic.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

varying vec4 fragmentColor;
varying vec2 fragmentUV;

uniform sampler2D sampler_1;

void main() {
    // Query the texture.
    vec4 color = texture2D(sampler_1, fragmentUV);
    
    //color.a = 0.5;
    
    // Blend color.
    //color *= fragmentColor;
    
    // TODO:
    //  - super fancy fragment shading :o
    
    // Final pixel color.
    gl_FragColor = color;//vec4(1, 0, 0, 0);//fragmentColor;
}
