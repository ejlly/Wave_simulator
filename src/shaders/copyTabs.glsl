//load shader with loadShaderPrefix

/*
#version 430

#define max_w 602
#define max_h 602
*/

layout (local_size_x = 32, local_size_y = 32) in;

layout (rgba32f, binding = 0) uniform image2D img_output;

layout (std430, binding = 1) coherent buffer SourceBuffer{
	float source[max_w][max_h];
	float u_2[max_w][max_h];
	float u_1[max_w][max_h];
	float u_0[max_w][max_h];
};


void main(){
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	if(coords.x == 0 || coords.x >= max_w-1 || coords.y == 0 || coords.y >= max_h-1){
		return;
	}

	u_2[coords.x][coords.y] = u_1[coords.x][coords.y];
	u_1[coords.x][coords.y] = u_0[coords.x][coords.y];
}
