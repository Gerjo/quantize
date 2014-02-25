//
//  basic.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//


in vec3 fragmentNormal;
in vec3 vertexPosition;
in vec2 fragmentUV;
in float fragmentSamplerIndex;


uniform sampler2D samplers[15];

uniform int lightCount;
uniform vec3 lightsPosition[10];
uniform vec4 lightsDiffuse[10];
uniform vec4 lightsSpecular[10];
uniform vec4 lightsAmbiant[10];

out vec4 finalColor;


void main() {

    //uniform sampler2D sampler_1 = samplers[0];

    int index = int(fragmentSamplerIndex);


    // Query the texture.
    vec4 texturecolor = texture(samplers[index], fragmentUV);
    
    
    vec3 normal = normalize(fragmentNormal);
    
    vec4 blend  = vec4(0.0, 0.0, 0.0, 0.0);
    
    for(int i = 0; i < lightCount; ++i) {
        vec3 lightPos     = lightsPosition[i];
        vec4 ambientColor = lightsAmbiant[i];
        vec4 diffuseColor = lightsDiffuse[i];
        vec4 specColor    = lightsSpecular[i];
        
        // Direction
        vec3 lightDir = normalize(lightPos - vertexPosition);

        float lambertian = max(dot(lightDir, normal), 0.0);
        float specular   = 0.0;

        if(lambertian > 0.0) {
            vec3 viewDir = normalize(-vertexPosition);

            // Blinn-Phong
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, 16.0);
            
        }
        
        // I'm guessing colors can be added. This seems intuitive, more lights
        // equals more light.
        blend = blend + (ambientColor + lambertian * diffuseColor + specular * specColor);
    }
   
    // todo: use 3 channel colors and neglect alpha?
    blend.a = texturecolor.a;
    
    //gl_FragColor = texturecolor * blend;
    finalColor = texturecolor * blend;
 }