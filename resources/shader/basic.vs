#version 330

in vec3 in_pos;
in vec2 in_uv;

uniform mat4 MVP;

out vec2 var_uv;

void main(){

	gl_Position=MVP*vec4(in_pos,1);
	var_uv=in_uv;

}