#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string.h>
#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "keys.hpp"
#include "shader_progs.hpp"

#define MY_ACTUAL_HEIGHT 700
#define MY_ACTUAL_WIDTH 700

#define HEIGHT (MY_ACTUAL_HEIGHT + 2)
#define WIDTH (MY_ACTUAL_WIDTH + 2)

#define CX 0.5//célérité
#define CY 0.5
#define EPS 0.00001
#define PI 3.1415926535897932384626

#define IMPULSE 1.1

/*
void update_tab(double **u_n, double **u_nm, bool **closed, SDL_Renderer *ren){
	double tmp(0);
	for(int i(1); i<HEIGHT+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			if(!closed[i][j]){
				//if(i == 50) printf("%d\n", j);
				tmp = -u_nm[i][j] + 2*u_n[i][j] + CX*CX * (u_n[i+1][j] - 2 * u_n[i][j] + u_n[i-1][j]) 
										   + CY*CY * (u_n[i][j+1] - 2 * u_n[i][j] + u_n[i][j-1]);
				u_nm[i][j] = u_n[i][j];
				u_n[i][j] = tmp;
			
				if(tmp > EPS){
					
					SDL_SetRenderDrawColor(ren, 0, 0, (int) 255*tmp/IMPULSE*.1 , 255);
				}
				else if(tmp < EPS && tmp > -EPS){
					SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
				}
				else{
					SDL_SetRenderDrawColor(ren, (int) 255*tmp/IMPULSE * 2, 0, 0, 255);	
				}
				SDL_RenderDrawPoint(ren, i-1, j-1);
			}
		}
	}
}

void update_tab2(double **u_n, double **u_nm, bool **closed){
	double tmp(0);
	for(int i(1); i<HEIGHT+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			if(!closed[i][j]){
			tmp = -u_nm[i][j] + 2*u_n[i][j] + CX*CX * (u_n[i+1][j] - 2 * u_n[i][j] + u_n[i-1][j]) 
										   + CY*CY * (u_n[i][j+1] - 2 * u_n[i][j] + u_n[i][j-1]);
			u_nm[i][j] = u_n[i][j];
			u_n[i][j] = tmp;
		}
		}
	}
}
*/

int main(int argc, char* argv[]){
	
	glfwInit();

	Window win(WIDTH, HEIGHT);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow *window(win.getaddr());

    window = glfwCreateWindow(WIDTH, HEIGHT, "Wave Simulation", nullptr, nullptr);
	win.setWindow(window);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WIDTH, HEIGHT);

	int const grp_size = 32;
	int const groups_x = (WIDTH/grp_size) + 1, groups_y = (HEIGHT/grp_size) + 1;

	std::string prefix("#version 430\n#define max_w ");
	prefix += std::to_string(WIDTH);
	prefix += "\n#define max_h ";
	prefix += std::to_string(HEIGHT);
	prefix += "\n";
	
	//std::cout << "PREFIX : " << prefix << std::endl;

	ComputeProgram copies("src/shaders/copyTabs.glsl", prefix.c_str());
	ComputeProgram calculate("src/shaders/computeWave.glsl", prefix.c_str());


	GLfloat source[4][WIDTH][HEIGHT]; //0 is source, 1 is u_2, 2 is u_1, 3 is u_0
	for(int i(0); i<WIDTH; i++){
		for(int j(0); j<HEIGHT; j++){
			for(int k(0); k<4; k++) source[k][i][j] = 0.f;
		}
	}


	GLuint tabBuffer[1];
	glGenBuffers(1, tabBuffer);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tabBuffer[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tabBuffer[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat)*4*WIDTH*HEIGHT, source, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint waveTextures[1];
	glGenTextures(1, waveTextures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waveTextures[0]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);  
	glBindImageTexture(0, waveTextures[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


	calculate.use();
	calculate.uniformf("CX", (GLfloat) CX);
	calculate.uniformf("CY", (GLfloat) CY);
	glUseProgram(0);
	
	GLfloat vertices[] = {
		// Positions		Colors			  Texture Coords
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Top Right
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Bottom Right
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
		-1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f // Top Left
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	DrawingProgram plot("src/shaders/vertexShader.vs", "src/shaders/fragmentShader.fs");

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2,	GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (6*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	GLfloat timeOrigin = glfwGetTime();

	copies.use();
	copies.compute(WIDTH, HEIGHT, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glUseProgram(0);

	int count(0);
		
	while(!glfwWindowShouldClose(win.getaddr())){
		glfwPollEvents();
		GLfloat timeValue = glfwGetTime();
		
		//compute part
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, waveTextures[0]);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, tabBuffer[0]);
			calculate.use();
			//calculate.uniformf("time", timeValue - timeOrigin);
			GLfloat* modify_source = (GLfloat*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
			if(modify_source){
				//modify_source[WIDTH/4 * HEIGHT + HEIGHT/4] = IMPULSE * sin(PI*(timeValue - timeOrigin)/1);
				//modify_source[3*WIDTH/4 * HEIGHT + 3*HEIGHT/4] = IMPULSE * sin(PI*(timeValue - timeOrigin)/1);


				//modify_source[WIDTH/4 * HEIGHT + HEIGHT/4] = IMPULSE; 
				//modify_source[3*WIDTH/4 * HEIGHT + 3*HEIGHT/4] = IMPULSE; 
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);



			calculate.compute(groups_x, groups_y, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			copies.use();
			copies.compute(groups_x, groups_y, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}


			

		glClear(GL_COLOR_BUFFER_BIT);
		plot.use();

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waveTextures[0]);
		
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(win.getaddr());

	}


	/*
	int mouse_x(0), mouse_y(0); //coordonnées de la souris
	
	double **u_n, **u_nm;
	bool **closed;

	u_n = new double *[HEIGHT+2];
	u_nm = new double *[HEIGHT+2];
	closed = new bool *[HEIGHT+2];

	for(int i(0); i<HEIGHT+2; i++){
		u_n[i] = new double[WIDTH+2];
		u_nm[i] = new double[WIDTH+2];
		closed[i]= new bool[WIDTH+2];
		for(int j(0); j<WIDTH+2; j++){
			u_n[i][j] = 0;
			u_nm[i][j] = 0;
			closed[i][j] = false;
		}
	}
	

	//SDL_Delay(2000);
	
	for(int alpha(0); alpha<HEIGHT; alpha++){
		if(!((alpha>HEIGHT/2-20 && alpha<HEIGHT/2-10) || (alpha>HEIGHT/2+10 && alpha<HEIGHT/2+20))){
			closed[WIDTH/2-100][alpha] = true;
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			SDL_RenderDrawPoint(ren, WIDTH/2-100, alpha);
		}
	}

	SDL_RenderPresent(ren);
	
	SDL_Delay(DT*1000);
		
	SDL_Event events;
	bool isOpen(true);
	
	printf("good\n");

	float counter(0);
	while(isOpen){
		while(SDL_PollEvent(&events)){
			switch(events.type){

				case SDL_QUIT: isOpen = false; printf("closing the sh\n"); break;
				
				case SDL_MOUSEMOTION:
					mouse_x = events.motion.x;
					mouse_y = events.motion.y;
					closed[mouse_x][mouse_y] = true;
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					SDL_RenderDrawPoint(ren, mouse_x, mouse_y);
					break;

				case SDL_MOUSEBUTTONDOWN:
					printf("+clic\n");
					break;
				
				case SDL_MOUSEBUTTONUP:
					printf("-clic\n");
					break;
			}
		}

		for(int l(0); l<128; l++){
			
			u_n[HEIGHT/2][WIDTH/2] = IMPULSE * sin(PI*counter/100);
			counter += DT/((float) 10);update_tab2(u_n, u_nm, closed);
			//SDL_RenderPresent(ren);
		}
		update_tab(u_n, u_nm, closed, ren);
		SDL_RenderPresent(ren);

		SDL_Delay(DT*100);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	*/

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	glDeleteTextures(1, waveTextures);
	glDeleteBuffers(1, tabBuffer);

	glfwTerminate();
	return 0;
}
