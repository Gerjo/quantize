//
//  basic.fsh
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

//precision mediump float;

varying vec3 normalInterp;
varying vec3 vertPos;
uniform sampler2D sampler_1;
varying vec2 fragmentUV;


const float ambient = 0.0;  // minimal light
const float diffuse = 1.8;
const float specular = 2.0;

const vec3 lightPos     = vec3(45.0, 25.0, 10.0);//vec3(1.0,1.0,1.0);
const vec3 ambientColor = vec3(ambient, ambient, ambient);
const vec3 diffuseColor = vec3(diffuse, diffuse, diffuse);
const vec3 specColor    = vec3(specular, specular, specular);

void main() {
    // Query the texture.
    vec4 texturecolor = texture2D(sampler_1, fragmentUV);
    
    
    vec3 normal   = normalize(normalInterp);
    vec3 lightDir = normalize(lightPos - vertPos);

    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular   = 0.0;

    if(lambertian > 0.0) {
        vec3 viewDir = normalize(-vertPos);

        // Blinn-phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, 16.0);
           
        // Phong shading
        // vec3 reflectDir = reflect(-lightDir, normal);
        // specAngle = max(dot(reflectDir, viewDir), 0.0);

        // Note that the exponent is different here
        // specular = pow(specAngle, 4.0);
    }

    gl_FragColor = texturecolor * vec4(ambientColor +
                      lambertian * diffuseColor +
                      specular * specColor, 1.0);
}


/*varying vec4 fragmentColor;
varying vec2 fragmentUV;


void main() {
    // Query the texture.
    vec4 color = texture2D(sampler_1, fragmentUV);
    
    //color.a = 0.5;
    
    // Blend color.
    //color *= fragmentColor;
    
    // TODO:
    //  - super fancy fragment shading :o
    
    // Final pixel color.
    gl_FragColor = color * fragmentColor;
}*/
