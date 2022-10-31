#version 430

layout (local_size_x = 32, local_size_y = 32) in;
layout (rgba32f, binding = 0) uniform image2D img_output;

layout (std430, binding = 1) coherent buffer SourceBuffer{
	float source[];
};

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
//uniform float time;

#define DT 1 //en ms
#define CX 0.005//célérité
#define CY 0.005
#define EPS 0.00001
#define PI 3.1415926535897932384626

#define IMPULSE (-1)

float minimum(float a, float b){if(a<b) return a; return b;}
//float abs(float a){ if(a>0) return a; return -a;}

float pixTofloat(vec4 pixel){
	return pixel.x - pixel.y;
}

vec4 floatToPix(float tmp){
	if(tmp > EPS){
		return vec4(minimum(-tmp/IMPULSE, 1.0f), 0.0f, 0.0f, 1.0f);
	}
	else if(tmp < EPS && tmp > -EPS){
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else{
		return vec4(0.0f, minimum(tmp/IMPULSE, 1.0f), 0.0f, 1.0f);	
	}
}

void main(){
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	if(coords.x == 0 || coords.x == max_w-1 || coords.y == 0 || coords.y == max_h-1){
		imageStore(img_output, coords, vec4(1.0f));
		return;
	}

	vec4 pixel;
	
	/*
	float mem_i_j  = pixTofloat(imageLoad(img_mem, coords));
	float cur_i_j  = pixTofloat(imageLoad(img_mem_intermediate, coords));
	float cur_ip_j = pixTofloat(imageLoad(img_mem_intermediate, ivec2(coords.x + 1u, coords.y)));
	float cur_im_j = pixTofloat(imageLoad(img_mem_intermediate, ivec2(coords.x - 1u, coords.y)));
	float cur_i_jp = pixTofloat(imageLoad(img_mem_intermediate, ivec2(coords.x, coords.y + 1u)));
	float cur_i_jm = pixTofloat(imageLoad(img_mem_intermediate, ivec2(coords.x, coords.y - 1u)));
	*/

	float mem_i_j  = u_2[coords.x * max_w + coords.y];
	float cur_i_j  = u_1[coords.x * max_w + coords.y];
	float cur_ip_j = u_1[(coords.x+1u) * max_w + coords.y];
	float cur_im_j = u_1[(coords.x-1u) * max_w + coords.y];
	float cur_i_jp = u_1[coords.x * max_w + (coords.y+1u)];
	float cur_i_jm = u_1[coords.x * max_w + (coords.y-1u)];

	float tmp = -mem_i_j + 2*cur_i_j + CX*CX * (cur_ip_j - 2 * cur_i_j + cur_im_j) 
									 + CY*CY * (cur_i_jp - 2 * cur_i_j + cur_i_jm);


	u_0[coords.x * max_w + coords.y] = tmp + source[coords.x * max_w + coords.y];

	pixel = floatToPix(u_0[coords.x * max_w + coords.y]);

	imageStore(img_output, coords, pixel);

	/*
	if ( coords.x %9 == 0 && coords.y %9 == 0) {
		pixel = vec4(1.0,.5,.0,1.0);
	}
	else {
		pixel = vec4(.0,.5,1.0,1.0);
	}
	
	//imageStore(img_output, coords, vec4(.0,.6,1.0, 1.0));

	/*
	for(int i = 0; i<600; i++){
		imageStore(img_output, ivec2(299u, i), vec4(.0,.6,1.0, 1.0));
	}
	*/
}
