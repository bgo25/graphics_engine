#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define YELLOW 0xFFFFFF00

/* Global helper parameters */
bool is_running = false;		// Flag to determine when quit
int win_height = 0;				// Total size of the rendered window: it
int win_width = 0;				// will be detected and fullscreen
uint32_t* color_buffer = NULL;	// Matrix of colors to be printed on screen

/* SDL structures */
SDL_Window* window = NULL;					// The window to be rendered
SDL_Renderer* renderer = NULL;				// The renderer
SDL_Texture* color_buffer_texture = NULL;	// Put colors in the renderer


/*	----------------------------------------------------------------
	Functions : they returns false if memory allocation problems
	----------------------------------------------------------------	*/

/* Initialize global variables "window", "renderer" and win_ height/width */
bool initialize_window (void)
{
	/* Initialize the basic SDL functionalities */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "ERR: SDL_Init was NOT successful.\n");
		return false;
	}

	/* Get the windows max size (fullscreen) using SDL API */
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	win_width = display_mode.w;
	win_height = display_mode.h;

	/* Create a SDL window centered on screen */
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		win_width,
		win_height,
		SDL_WINDOW_BORDERLESS
	);

	if (window == NULL) {
		fprintf(stderr, "ERR: SDL_CreateWindow was NOT successful.\n");
		return false;
	}
	/* Create a renderer inside the window */
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		fprintf(stderr, "ERR: SDL_CreateRenderer was NOT successful.\n");
		return false;
	}
	/* Tell SDL to actually use "true" FULL SCREEN */
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	return true;
}

/* Initialize global variables color_buffer and color_buffer_texture */ 
bool setup (void)
{
	/* Allocate memory for the frame buffer, i.e. array with colors */
	color_buffer = malloc(win_height * win_width * sizeof *color_buffer);
	if (color_buffer == NULL)
	{
		fprintf(stderr, "ERR: malloc failed on color_buffer.\n");
		return false;
	}
	/* Fill the color buffer with hex code black, to have valid values */
	for (int i = 0; i < win_height * win_width; ++i ){
		color_buffer[i] = 0xFFFFFFFF;
	}
	/* Allocate color_buffer_texture and connect with renderer */
	color_buffer_texture = SDL_CreateTexture (
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		win_width,
		win_height
	);
	return true;
}

/* Reads possible user input and modify is_running appropriately */
void process_input (void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		// If window is closed
		case SDL_QUIT:
			is_running = false;
		break;
		// If ESC button is pressed
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
		break;
	}
}

/* TO IMPLEMENT */
void update (void)
{

}

/* Copy the content of color_buffer into color_buffer_texture. The former
   was the global array where we can manually, pixel by pixel, set the
   color, while the latter can be used from SDL to actually display */
void render_color_buffer(void)
{
	SDL_UpdateTexture (
		color_buffer_texture,
		NULL,
		color_buffer,
		(int) (win_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);	
}

/* Reset the color buffer to a single color  */
bool clear_color_buffer(uint32_t color)
{
	if (color_buffer == NULL) {
		fprintf(stderr, "clear_color_buffer: color_buffer is NULL!\n");
		return false;
	}
	for (int y = 0; y < win_height; ++y) {
		for (int x = 0; x < win_width; ++x) {
			color_buffer[y * win_width + x] = color;
		}
	}

	return true;
}

/* Modify the color_buffer so that is contains a grid */
bool draw_grid(uint32_t color)
{
	if (color_buffer == NULL) {
		fprintf(stderr, "draw_grid: color_buffer is NULL!\n");
		return false;
	}
	for (int y = 0; y < win_height; ++y) {
		for (int x = 0; x < win_width; ++x) {
			if ((y % 10 == 0) || (x % 10 == 0)) {
				color_buffer[y * win_width + x] = color;
			}
		}
	}
	return true;
}


bool draw_rect_full(int x, int y, int heigth, int width, uint32_t color)
{
	/* First, check that the numbers are compatible with window size */
	if (x + width > win_width) {
		fprintf(stderr, "Rectangle over max width.");
		return false;
	}
	if (y + heigth > win_height) {
		fprintf(stderr, "Rectangle over max heigth.");
		return false;
	}
	/* Otherwise, print the rectangle, i.e. set color_buffer to values */
	/* Go in position (x, y) */
//	start_idx = win_width * y + x;
	for (int curr_h = 0; curr_h < heigth; ++curr_h) {
		for (int curr_w = 0; curr_w < width; ++curr_w) {
			/* Fill in color at the current position */
			color_buffer[win_width * (y + curr_h) + (x + curr_w)] = color;
		}
	}
	return true;
}


void render (void)
{
	/* Render only red, with no transparency */
//	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//	SDL_RenderClear(renderer);

	/* Reset the background */
	clear_color_buffer(BLACK);
	/* On it, write the picture */
	draw_grid(BLUE);
	draw_rect_full(10, 20, 50, 400, YELLOW);
	draw_rect_full(70, 70, 100, 45, RED);

	/* Render the overall result */
	render_color_buffer();
	SDL_RenderPresent(renderer);

}

void destroy_window(void)
{
	if (color_buffer != NULL) {
		free(color_buffer);
	} 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(void)
{
	is_running = initialize_window();

	if (!setup()) {
		fprintf(stderr, "Unable to proceed.\n");
		return 1;
	}

	while (is_running) {
		process_input();
		update();
		render();
	}
	destroy_window();
    return 0;
}
