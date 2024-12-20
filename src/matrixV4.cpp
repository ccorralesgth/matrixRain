#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FONT_SIZE = 16;
const int NUM_COLUMNS = SCREEN_WIDTH / FONT_SIZE;
const int NUM_LETTERS_PER_COLUMN = 20;
const int LAYERS = 3;

struct Letter {
    int x, y;
    char character;
    int speed;
    SDL_Color color;
    int alpha; // Opacity for fading
};

SDL_Color getRandomColor() {
    return {static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), 255};
}

char getRandomASCIICharacter() {
    return static_cast<char>(32 + (rand() % (126 - 32))); // Printable ASCII range
}

void renderLetter(SDL_Renderer* renderer, TTF_Font* font, char character, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, std::string(1, character).c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {x, y, FONT_SIZE, FONT_SIZE};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        SDL_Log("Failed to initialize TTF: %s", TTF_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Matrix Falling Letters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("res/fonts/NES-Chimera.ttf", FONT_SIZE);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return -1;
    }

    std::srand(std::time(nullptr));

    std::vector<Letter> letters;

    // Initialize letters for all columns
    for (int col = 0; col < NUM_COLUMNS; ++col) {
        for (int i = 0; i < NUM_LETTERS_PER_COLUMN; ++i) {
            Letter letter;
            letter.x = col * FONT_SIZE;
            letter.y = -i * FONT_SIZE; // Start letters off-screen
            letter.character = getRandomASCIICharacter();
            letter.speed = 10 + rand() % 5;
            letter.color = getRandomColor();
            letter.alpha = 255; // Fully opaque
            letters.push_back(letter);
        }
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT  || (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN)) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render letters
        for (auto& letter : letters) {
            if (letter.alpha > 0) {
                letter.color.a = letter.alpha; // Apply fading
                renderLetter(renderer, font, letter.character, letter.x, letter.y, letter.color);
            }

            // Update letter position and fade
            letter.y += letter.speed;
            letter.alpha -= rand() % 5; // Random fade speed

            if (letter.y > SCREEN_HEIGHT || letter.alpha <= 0) {
                // Reset letter to top
                letter.y = -FONT_SIZE * (rand() % 10);
                letter.character = getRandomASCIICharacter();
                letter.speed = 10 + rand() % 5;
                letter.color = getRandomColor();
                letter.alpha = 255;
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
