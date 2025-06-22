#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int win_height = 600;
int win_width = 800;

bool initialize_window (void)
{
	/* Initialize the basic SDL functionalities */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "ERR: SDL_Init was NOT successful.\n");
		return false;
	}
	/* Create a SDL window */
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
	return true;
}

bool setup (void)
{
	/* Allocate memory for the frame buffer, i.e. array with colors */
	color_buffer = malloc(win_height * win_width * sizeof *color_buffer);
	if (color_buffer == NULL)
	{
		fprintf(stderr, "ERR: malloc failed on color_buffer.\n");
		return false;
	}
	/* Mirrors my color into the appropriate SDL Structure */
	color_buffer_texture = SDL_CreateTexture (
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		win_width,
		win_height
	);
	
	return true;
}

void process_input (void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
			is_running = false;
		break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
		break;
	}
}

void update (void)
{

}

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

void render (void)
{
	/* Render only red, with no transparency */
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	render_color_buffer();
	clear_color_buffer(0xFFFFFF00);


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

