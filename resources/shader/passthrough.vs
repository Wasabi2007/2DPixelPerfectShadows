#version 330
// Input vertex data, different for all executions of this shader.
in vec3 in_pos;

// Output data ; will be interpolated for each fragment.
out vec2 var_uv;

void main(){
	gl_Position =  vec4(in_pos,1);
	var_uv = (in_pos.xy+vec2(1,1))/2.0;
}