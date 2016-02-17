#version 330

in vec4 in_pos;
in vec2 in_uv;

uniform mat4 MVP;

out vec2 var_uv;

void main(){

	gl_Position=MVP*in_pos;
	var_uv=in_uv;

}