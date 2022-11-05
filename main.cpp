#include <SDL2/SDL.h>

#define LENGTH 300
#define WIDTH 300

#define DT 1 //en ms
#define CX 0.005//célérité
#define CY 0.005
#define EPS 0.00001
#define PI 3.1415926535897932384626

#define IMPULSE (-1)

using namespace std;

void update_tab(double **u_n, double **u_nm, double **u_nmm, double **source, bool **closed, SDL_Renderer *ren){
	double tmp(0);
	for(int i(1); i<LENGTH+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			if(!closed[i][j]){
				//if(i == 50) printf("%d\n", j);
				tmp = -u_nmm[i][j] + 2*u_nm[i][j] + CX*CX * (u_nm[i+1][j] - 2 * u_nm[i][j] + u_nm[i-1][j]) 
										   + CY*CY * (u_nm[i][j+1] - 2 * u_nm[i][j] + u_nm[i][j-1]) + source[i][j];

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
	
	for(int i(1); i<LENGTH+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			u_nmm[i][j] = u_nm[i][j];
			u_nm[i][j] = u_n[i][j];
		}
	}
	
}

void update_tab2(double **u_n, double **u_nm, double **u_nmm, double **source, bool **closed){
	double tmp(0);
	for(int i(1); i<LENGTH+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			if(!closed[i][j]){
			tmp = -u_nmm[i][j] + 2*u_nm[i][j] + CX*CX * (u_nm[i+1][j] - 2 * u_nm[i][j] + u_nm[i-1][j]) 
										   + CY*CY * (u_nm[i][j+1] - 2 * u_nm[i][j] + u_nm[i][j-1]) + source[i][j];
			u_n[i][j] = tmp;

			}
		}
	}

	for(int i(1); i<LENGTH+1; i++){
		for(int j(1); j<WIDTH+1; j++){
			u_nmm[i][j] = u_nm[i][j];
			u_nm[i][j] = u_n[i][j];
		}
	}
}


int main(int argc, char* argv[]){
	
	printf("dioaj\n");

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
    }
    
	SDL_Window *win = SDL_CreateWindow("Wave",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       LENGTH, WIDTH, SDL_WINDOW_RESIZABLE);
	
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
												 SDL_TEXTUREACCESS_STREAMING,
												 LENGTH,
												 WIDTH);

	unsigned char pixels[WIDTH*LENGTH*4];

	int mouse_x(0), mouse_y(0); //coordonnées de la souris
	
	double **u_n, **u_nm, **u_nmm, **source;
	bool **closed;

	u_n = new double *[LENGTH+2];
	u_nm = new double *[LENGTH+2];
	u_nmm = new double *[LENGTH+2];
	source = new double *[LENGTH+2];
	closed = new bool *[LENGTH+2];

	for(int i(0); i<LENGTH+2; i++){
		u_n[i] = new double[WIDTH+2];
		u_nm[i] = new double[WIDTH+2];
		u_nmm[i] = new double[WIDTH+2];
		source[i] = new double[WIDTH+2];
		closed[i]= new bool[WIDTH+2];
		for(int j(0); j<WIDTH+2; j++){
			u_n[i][j] = 0;
			u_nm[i][j] = 0;
			u_nmm[i][j] = 0;
			source[i][j] = 0;
			closed[i][j] = false;
		}
	}
	

	//SDL_Delay(2000);
	
	for(int alpha(0); alpha<LENGTH; alpha++){
		if(!((alpha>LENGTH/2-20 && alpha<LENGTH/2-10) || (alpha>LENGTH/2+10 && alpha<LENGTH/2+20))){
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
			
			source[LENGTH/2][WIDTH/2] = IMPULSE * sin(PI*counter/100);
			counter += DT/((float) 10);update_tab2(u_n, u_nm, u_nmm, source, closed);
			//SDL_RenderPresent(ren);
		}
		update_tab(u_n, u_nm, u_nmm, source, closed, ren);
		SDL_RenderPresent(ren);

		SDL_Delay(DT*100);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
