//
//  Shader.vsh
//  derp
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

attribute vec3 position;
uniform mat4 camera;

//varying lowp vec4 colorVarying;
//varying vec4 colorVarying;


void main()
{
    gl_Position = vec4(position, 1) * camera;
}
