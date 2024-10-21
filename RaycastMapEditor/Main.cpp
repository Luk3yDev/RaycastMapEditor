#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

const int tileSize = 32;
const int mapWidth = 25;
const int mapHeight = 25;

const int TILE_TYPES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const int numTileTypes = sizeof(TILE_TYPES) / sizeof(TILE_TYPES[0]);

int currentTileIndex = 0;
std::vector<std::vector<int>> worldMap(mapHeight, std::vector<int>(mapWidth, 0));

SDL_Texture* textures[numTileTypes];

struct Sprite
{
    double x;
    double y;
    int texIndex;
    SDL_Surface* texture;
};

Sprite sprite[256];

void loadTextures(SDL_Renderer* renderer) {
    for (int i = 1; i < numTileTypes; i++) {
        std::string fileName = "walls/tile_" + std::to_string(i) + ".bmp";
        SDL_Surface* rawImage = SDL_LoadBMP(fileName.c_str());        
        textures[i] = SDL_CreateTextureFromSurface(renderer, rawImage);
        if (!textures[i]) {
            std::cerr << "Failed to load wall texture! SDL_Error: " << SDL_GetError() << std::endl;
        }
    }
}

void drawGrid(SDL_Renderer* renderer) {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            SDL_Rect rect = { x * tileSize, y * tileSize, tileSize, tileSize };
            int tileType = worldMap[y][x];

            if (tileType >= 0 && tileType < numTileTypes) {
                SDL_RenderCopy(renderer, textures[tileType], nullptr, &rect);
            }
            
            SDL_SetRenderDrawColor(renderer, 150, 50, 200, 255);
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

    loadTextures(renderer);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentTileIndex = (currentTileIndex + 1) % numTileTypes;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentTileIndex = (currentTileIndex - 1 + numTileTypes) % numTileTypes;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    saveMap("map.rmap");
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouseX = event.button.x / tileSize;
                    int mouseY = event.button.y / tileSize;
                    if (mouseX < mapWidth && mouseY < mapHeight) {
                        worldMap[mouseY][mouseX] = TILE_TYPES[currentTileIndex];
                    }
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    int mouseX = event.button.x / tileSize;
                    int mouseY = event.button.y / tileSize;
                    if (mouseX < mapWidth && mouseY < mapHeight) {
                        worldMap[mouseY][mouseX] = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 15, 4, 15, 255);
        SDL_RenderClear(renderer);
        drawGrid(renderer);
        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < numTileTypes; ++i) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}