#version 330

#define PI 3.14159265359

//inputs from vertex shader
in vec2 var_uv;

//uniform values
uniform sampler2D shadow_map_texture;
uniform vec2 light_resolution;
uniform vec4 Color;

//usability stuff
uniform float selected;
uniform float blur_factor;


out vec4 out_color;

//sample from the 1D distance map
float sample(float coord, float r) {
    return step(r, texture(shadow_map_texture, vec2(coord,0)).r);
}

void main(void) {
    //rectangular to polar
    vec2 norm = var_uv.st * 2.0 - 1.0;
    float theta = atan(norm.y, norm.x);
    float r = length(norm);
    float coord = 1-(theta + PI) / (2.0*PI);

    //the tex coord to sample our 1D lookup texture
    //always 0.0 on y axis
    vec2 tc = vec2(coord, 0.0);

    //the center tex coord, which gives us hard shadows
    float center = sample(coord, r);

    //we multiply the blur amount by our distance from center
    //this leads to more blurriness as the shadow "fades away"
    float blur = (1./light_resolution.x) * smoothstep(0., 1., r);

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

    sum = (1-blur_factor)*sum + (blur_factor)*center;

    //multiply the summed amount by our distance, which gives us a radial falloff
    //then multiply by vertex (light) color
    out_color = Color * vec4(vec3(1.0), sum * ((1-selected)*smoothstep(1.0, 0.0, r)+(selected)));
}