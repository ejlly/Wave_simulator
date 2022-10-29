#version 430

uniform sampler2D img_output;

//in vec3 ourColor;
in vec2 TexCoord;


out vec4 color;

void main(){
	color = texture(img_output, TexCoord);
	//color = vec4(ourColor, 1.0f);
}
