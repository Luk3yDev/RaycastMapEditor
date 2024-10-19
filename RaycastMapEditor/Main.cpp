#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>

const int tileSize = 32;
const int mapWidth = 24;
const int mapHeight = 24;

const int TILE_TYPES[] = { 0, 1, 2, 3, 4, 5, 6 };
const int numTileTypes = sizeof(TILE_TYPES) / sizeof(TILE_TYPES[0]);

int currentTileIndex = 0; // Index for the current tile type
std::vector<std::vector<int>> worldMap(mapHeight, std::vector<int>(mapWidth, 0));

void drawGrid(SDL_Renderer* renderer) {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            SDL_Rect rect = { x * tileSize, y * tileSize, tileSize, tileSize };
            int tileType = worldMap[y][x];
            switch (tileType) {
            case 0: SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); break;
            case 1: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
            case 2: SDL_SetRenderDrawColor(renderer, 100, 150, 100, 255); break;
            case 3: SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); break;
            case 4: SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); break;
            case 5: SDL_SetRenderDrawColor(renderer, 135, 135, 135, 255); break;
            case 6: SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255); break;
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
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentTileIndex = (currentTileIndex + 1) % numTileTypes; // Scroll right
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentTileIndex = (currentTileIndex - 1 + numTileTypes) % numTileTypes; // Scroll left
                }
                if (event.key.keysym.sym == SDLK_s) {
                    saveMap("map.txt");
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x / tileSize;
                int mouseY = event.button.y / tileSize;
                if (mouseX < mapWidth && mouseY < mapHeight) {
                    worldMap[mouseY][mouseX] = TILE_TYPES[currentTileIndex]; // Set the selected tile type
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