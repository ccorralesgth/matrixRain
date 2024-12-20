#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FONT_SIZE = 24; // Larger font size for readability
const int NUM_COLUMNS = SCREEN_WIDTH / FONT_SIZE;
const int NUM_LETTERS_PER_COLUMN = 20;

// Randomly generate a Unicode CJK character
std::wstring getRandomCJKCharacter() {
    int ranges[][2] = {
        {0x4E00, 0x9FFF}, // Chinese
        {0x30A0, 0x30FF}, // Japanese Katakana
        {0xAC00, 0xD7AF}  // Korean Hangul
    };
    int rangeIndex = rand() % 3;
    int start = ranges[rangeIndex][0];
    int end = ranges[rangeIndex][1];
    wchar_t wc = start + (rand() % (end - start + 1));
    return std::wstring(1, wc);
}

void renderLetter(SDL_Renderer* renderer, TTF_Font* font, const std::wstring& character, int x, int y, SDL_Color color) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string utf8_char = converter.to_bytes(character);

    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, utf8_char.c_str(), color);
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

    SDL_Window* window = SDL_CreateWindow("Matrix Falling Letters with CJK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return -1;
    }

    // TTF_Font* font = TTF_OpenFont("res/fonts/NES-Chimera.ttf", FONT_SIZE);
     TTF_Font* font = TTF_OpenFont("res/fonts/NotoSerifCJK-VF.ttf.ttc", FONT_SIZE);
    // TTF_Font* font3 = TTF_OpenFont("res/fonts/NotoSerifCJKjp-VF.ttf", FONT_SIZE);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return -1;
    }

    std::srand(std::time(nullptr));

    struct Letter {
        int x, y;
        std::wstring character;
        int speed;
        SDL_Color color;
        int alpha; // Opacity for fading
    };

    std::vector<Letter> letters;

    // Initialize letters for all columns
    for (int col = 0; col < NUM_COLUMNS; ++col) {
        for (int i = 0; i < NUM_LETTERS_PER_COLUMN; ++i) {
            Letter letter;
            letter.x = col * FONT_SIZE;
            letter.y = -i * FONT_SIZE; // Start letters off-screen
            letter.character = getRandomCJKCharacter();
            letter.speed = 2 + rand() % 5;
            letter.color = {0, 255, 0, 255};
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
                letter.character = getRandomCJKCharacter();
                letter.speed = 2 + rand() % 5;
                letter.color = {0, 255, 0, 255};
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
