#version 330

in vec2 var_uv;

uniform sampler2D tex;

out vec4 out_color;

void main(){
	out_color=texture(tex,var_uv);
}