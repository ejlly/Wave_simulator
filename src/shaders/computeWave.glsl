#version 430

layout (local_size_x = 32, local_size_y = 32) in;
layout (rgba32f, binding = 0) uniform image2D img_output;

uniform uint max_w;
uniform uint max_h;
uniform float time;

#define DT 1 //en ms
#define CX 0.005//célérité
#define CY 0.005
#define EPS 0.00001
#define PI 3.1415926535897932384626

#define IMPULSE (-1)

void main(){
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	if(coords.x == 0 || coords.x == max_w-1 || coords.y == 0 || coords.y == max_h-1){
		imageStore(img_output, coords, vec4(1.0f));
		return;
	}

	vec4 pixel;

	tmp = -u_nm[i][j] + 2*u_n[i][j] + CX*CX * (u_n[i+1][j] - 2 * u_n[i][j] + u_n[i-1][j]) 
							   + CY*CY * (u_n[i][j+1] - 2 * u_n[i][j] + u_n[i][j-1]);
	u_nm[i][j] = u_n[i][j];
	u_n[i][j] = tmp;

	if(tmp > EPS){
		pixel = vec4(0.0f, 0.0f, tmp/IMPULSE*.1 , 1.0f);
	}
	else if(tmp < EPS && tmp > -EPS){
		pixel = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else{
		pixel = vec4(tmp/IMPULSE * 2, 0.0f, 0.0f, 1.0f);	
	}


	/*
	if ( coords.x %9 == 0 && coords.y %9 == 0) {
		pixel = vec4(1.0,.5,.0,1.0);
	}
	else {
		pixel = vec4(.0,.5,1.0,1.0);
	}
	
	imageStore(img_output, coords, pixel);
	//imageStore(img_output, coords, vec4(.0,.6,1.0, 1.0));

	/*
	for(int i = 0; i<600; i++){
		imageStore(img_output, ivec2(299u, i), vec4(.0,.6,1.0, 1.0));
	}
	*/
}
