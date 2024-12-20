#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FONT_SIZE = 12;
const int NUM_COLUMNS = SCREEN_WIDTH / FONT_SIZE;
const int MAX_ROW_LETTER = SCREEN_HEIGHT / FONT_SIZE;

struct Letter
{
	int x, y;
	char character;
	int speed;
};

void renderLetter(SDL_Renderer *renderer, TTF_Font *font, char character, int x, int y)
{
	SDL_Color green = {0, 255, 0, 255};
	SDL_Surface *surface = TTF_RenderText_Solid(font, std::string(1, character).c_str(), green);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect dstRect = {x, y, FONT_SIZE, FONT_SIZE};
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[])
{
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	TTF_Font *font = nullptr;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return -1;
	}

	if (TTF_Init() < 0)
	{
		SDL_Log("Failed to initialize TTF: %s", TTF_GetError());
		return -1;
	}

	window = SDL_CreateWindow("Matrix Falling Letters",
							  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	// SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN); //maximized window
	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return -1;
	}

	font = TTF_OpenFont("res/fonts/NES-Chimera.ttf", FONT_SIZE);
	if (!font)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
		return -1;
	}

	std::srand(std::time(nullptr));

	std::vector<Letter> letters;

	// Initialize letters
	for (int col = 0; col < NUM_COLUMNS; ++col)
	{
		Letter letter;
		int max_letters = std::rand() % MAX_ROW_LETTER;
		letter.speed = 2 + std::rand() % 5;
		letter.x = col * FONT_SIZE;
		for (int j = 0; j < max_letters; j++)
		{
			letter.y = -j * FONT_SIZE * 10;
			letter.character = 'A' + std::rand() % 26;
			letters.push_back(letter);
		}
	}

	bool running = true;
	SDL_Event event;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN))
			{
				running = false;
			}
		}

		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Render letters
		for (auto &letter : letters)
		{
			// if (letter.alpha > 0)
			// {
			// 	letter.color.a = letter.alpha; // Apply fading
			// 	renderLetter(renderer, font, letter.character, letter.x, letter.y);
			// }
			renderLetter(renderer, font, letter.character, letter.x, letter.y);
			// Update position
			letter.y += letter.speed;
			// Reset letter if it moves out of screen or fully fades
			if (letter.y > SCREEN_HEIGHT)
			{
				letter.y = -FONT_SIZE * (rand() % 10); // Reset to top
				letter.character = 'A' + std::rand() % 26;
				letter.speed = 2 + rand() % 5;
			}
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16); // ~60 FPS
	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
