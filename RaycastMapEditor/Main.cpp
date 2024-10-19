#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>

#define mapWidth 24
#define mapHeight 24

const int tileSize = 32;

std::vector<std::vector<int>> worldMap(mapHeight, std::vector<int>(mapWidth, 0));

void drawGrid(SDL_Renderer* renderer) {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            SDL_Rect rect = { x * tileSize, y * tileSize, tileSize, tileSize };
            switch (worldMap[y][x]) {
            case 1: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break; // Wall
            case 0: SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); break; // Empty
            default: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break; // Custom tile
            }
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Grid color
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}

void saveMap(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "int worldMap[" << mapHeight << "][" << mapWidth << "] = {\n";
        for (int y = 0; y < mapHeight; y++) {
            file << "  {";
            for (int x = 0; x < mapWidth; x++) {
                file << worldMap[y][x];
                if (x < mapWidth - 1) file << ",";
            }
            file << "}";
            if (y < mapHeight - 1) file << ",";
            file << "\n";
        }
        file << "};\n";
        file.close();
        std::cout << "Map saved to " << filename << "\n";
    }
    else {
        std::cerr << "Could not open file for writing.\n";
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mapWidth * tileSize, mapHeight * tileSize, 0);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_s) {
                    saveMap("map.txt");
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x / tileSize;
                int mouseY = event.button.y / tileSize;
                if (mouseX < mapWidth && mouseY < mapHeight) {
                    worldMap[mouseY][mouseX] = (worldMap[mouseY][mouseX] + 1) % 2; // Toggle tile
                }
            }
        }

        SDL_RenderClear(renderer);
        drawGrid(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}