#version 330

#define PI 3.14159265359
in vec2 var_uv;

uniform sampler2D ocluder_texture;
uniform vec2 light_resolution;


//alpha threshold for our occlusion map
const float ALPHA_THRESHOLD = 0.75;

out vec4 out_color;

void main(){
	float distance = 1.0;

	for (float y=0.0; y<light_resolution.y; y+=1.0) {
		//rectangular to polar filter
		vec2 norm = vec2(var_uv.s, y/light_resolution.y) * 2.0 - 1.0;
		float theta = PI*1.5 + norm.x * PI;
		float r = (1.0 + norm.y) * 0.5;

		//Coordinat which we will sample from occlude map
        vec2 coord = vec2(-r * sin(theta), -r * cos(theta))/2.0 + 0.5;

		//sample the occlusion map
		vec4 data = texture2D(ocluder_texture, coord);

        //the current distance is how far from the top we've come
        float dst = y/light_resolution.y;

        //if we've hit an opaque fragment (occluder), then get new distance
		//if the new distance is below the current, then we'll use that for our ray
		float caster = data.a;
		if(isnan(caster)) caster = 1;

		if (caster > ALPHA_THRESHOLD) {
			distance = min(distance, dst);
		}
	}
	out_color=vec4(vec3(distance), 1.0);
}