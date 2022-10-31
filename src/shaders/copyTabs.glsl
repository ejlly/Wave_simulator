#version 430

layout (local_size_x = 32, local_size_y = 32) in;
layout (std430, binding = 2) coherent buffer MemBuffer{
	float u_2[];
};

layout (std430, binding = 3) coherent buffer MemIntermediateBuffer{
	float u_1[];
};

layout (std430, binding = 4) coherent buffer MemCur{
	float u_0[];
};

uniform uint max_w;
uniform uint max_h;

void main(){
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	if(coords.x == 0 || coords.x == max_w-1 || coords.y == 0 || coords.y == max_h-1){
		return;
	}

	u_2[coords.x + max_w + coords.y] = u_1[coords.x + max_w + coords.y];
	u_1[coords.x + max_w + coords.y] = u_0[coords.x + max_w + coords.y];
}
