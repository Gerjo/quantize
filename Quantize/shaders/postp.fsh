//
//  postp.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

uniform sampler2D texture;
varying vec2 uvmapping;
 
void main() {
    gl_FragColor = texture2D(texture, uvmapping);
}
