//load shader with loadShaderPrefix

/*
#version 430

#define max_w 602
#define max_h 602
*/

#define EPS 0.00001

layout (local_size_x = 32, local_size_y = 32) in;
layout (rgba32f, binding = 0) uniform image2D img_output;


layout (std430, binding = 1) coherent buffer SourceBuffer{
	float source[max_w][max_h];
	float u_2[max_w][max_h];
	float u_1[max_w][max_h];
	float u_0[max_w][max_h];
};

//uniform float time;

uniform float CX;//célérité
uniform float CY;



float minimum(float a, float b){if(a<b) return a; return b;}
float abs(float a){if(a<0) return -a; return a;}

vec4 floatToPix(float tmp){
	if(tmp > EPS){
		return vec4(minimum(abs(tmp), 1.0f), 0.0f, 0.0f, 1.0f);
	}
	else if(tmp < EPS && tmp > -EPS){
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else{
		return vec4(0.0f, minimum(abs(tmp), 1.0f), 0.0f, 1.0f);	
	}
}

void main(){
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	if(coords.x == 0 || coords.x == max_w-1 || coords.y == 0 || coords.y == max_h-1){
		imageStore(img_output, coords, vec4(1.0f));
		return;
	}

	vec4 pixel;
	

	float mem_i_j  = u_2[coords.x][coords.y];
	float cur_i_j  = u_1[coords.x][coords.y];
	float cur_ip_j = u_1[coords.x+1][coords.y];
	float cur_im_j = u_1[coords.x-1][coords.y];
	float cur_i_jp = u_1[coords.x][coords.y+1];
	float cur_i_jm = u_1[coords.x][coords.y-1]; 
	
	float tmp = -mem_i_j + 2*cur_i_j + CX*CX * (cur_ip_j - 2 * cur_i_j + cur_im_j) 
									 + CY*CY * (cur_i_jp - 2 * cur_i_j + cur_i_jm);

	u_0[coords.x][coords.y] = tmp + source[coords.x][coords.y];

	pixel = floatToPix(u_0[coords.x][coords.y]);

	imageStore(img_output, coords, pixel);
}
