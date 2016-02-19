#version 330

#define PI 3.14159265359

//inputs from vertex shader
varying vec2 var_uv;

//uniform values
uniform sampler2D shadow_map_texture;
uniform vec2 resolution;
uniform vec4 Color;

//sample from the 1D distance map
float sample(float coord, float r) {
    return step(r, texture(shadow_map_texture, vec2(coord,0)).r);
}

void main(void) {
    //rectangular to polar
    vec2 norm = var_uv.st * 2.0 - 1.0;
    float theta = atan(norm.y, norm.x);
    float r = length(norm);
    float coord = (theta + PI) / (2.0*PI);

    //the tex coord to sample our 1D lookup texture
    //always 0.0 on y axis
    vec2 tc = vec2(coord, 0.0);

    //the center tex coord, which gives us hard shadows
    float center = sample(1-coord, r);

    //we multiply the blur amount by our distance from center
    //this leads to more blurriness as the shadow "fades away"
    float blur = (1./resolution.x)  * smoothstep(0., 1., r);

    //now we use a simple gaussian blur
    float sum = 0.0;

    sum += sample(coord - 4.0*blur, r) * 0.05;
    sum += sample(coord - 3.0*blur, r) * 0.09;
    sum += sample(coord - 2.0*blur, r) * 0.12;
    sum += sample(coord - 1.0*blur, r) * 0.15;

    sum += center * 0.16;

    sum += sample(coord + 1.0*blur, r) * 0.15;
    sum += sample(coord + 2.0*blur, r) * 0.12;
    sum += sample(coord + 3.0*blur, r) * 0.09;
    sum += sample(coord + 4.0*blur, r) * 0.05;

    //sum of 1.0 -> in light, 0.0 -> in shadow

    //multiply the summed amount by our distance, which gives us a radial falloff
    //then multiply by vertex (light) color
    gl_FragColor = Color * vec4(vec3(1.0), sum * smoothstep(1.0, 0.0, r));
}