#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "maze_3D.h"


/**
 * gameLoop - The main game loop.
 * @game: The game state.
 */
void gameLoop(Game *game)
{
	ColorRGB RGB_Red = {255, 0, 0};
	ColorRGB RGB_Green = {0, 255, 0};
	ColorRGB RGB_Blue = {0, 0, 255};
	ColorRGB RGB_White = {255, 255, 255};
	ColorRGB RGB_Yellow = {255, 255, 0};
	ColorRGB RGB_Grey = {128, 128, 128};

	int (*worldMap)[mapWidth][mapHeight] = getWorldMap();

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
		}
		SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
		SDL_RenderClear(game->renderer);

		for (int x = 0; x < screenWidth; x++)
		{
			float cameraX = 2 * x / (float)screenWidth - 1;
			float rayDirX = game->dirX + game->planeX * cameraX;
			float rayDirY = game->dirY + game->planeY * cameraX;
			int mapX = (int)(game->posX);
			int mapY = (int)(game->posY);
			float sideDistX;
			float sideDistY;
			float deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			float deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			float perpWallDist;
			int stepX;
			int stepY;
			int hit = 0;
			int side;
			if (rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (game->posX - mapX) * deltaDistX;
            }
			else
			{
                stepX = 1;
                sideDistX = (mapX + 1.0 - game->posX) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (game->posY - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - game->posY) * deltaDistY;
            }
            while (hit == 0) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if ((*worldMap)[mapX][mapY] > 0)
                    hit = 1;
            }
            if (side == 0)
                perpWallDist = fabs((mapX - game->posX + (1 - stepX) / 2) / rayDirX);
            else
                perpWallDist = fabs((mapY - game->posY + (1 - stepY) / 2) / rayDirY);
            int h = screenHeight;
            int lineHeight = (int)(3 * h / perpWallDist);
            int drawStart = -lineHeight / 2 + h / 2;
            if (drawStart < 0)
                drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if (drawEnd >= h)
                drawEnd = h - 1;
            ColorRGB color;
            switch ((*worldMap)[mapX][mapY]) {
            case 1:
                color = RGB_Red;
                break;
            case 2:
                color = RGB_Green;
                break;
            case 3:
                color = RGB_Blue;
                break;
            case 4:
                color = RGB_White;
                break;
            default:
                color = RGB_Yellow;
                break;
            }
            if (side == 1) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }
            verLine(game->renderer, x, drawStart, drawEnd, color);
            SDL_SetRenderDrawColor(game->renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawLine(game->renderer, x, drawStart, x, drawEnd);
        }

        game->oldTime = game->time;
        game->time = SDL_GetTicks();
        float frameTime = (game->time - game->oldTime) / 1000.0;

        char fpsText[16];
        sprintf(fpsText, "FPS: %.2f", 1.0 / frameTime);
        SDL_Color textColor = {255, 255, 255, 255};
        renderText(game->renderer, game->font, fpsText, textColor, 10, 10);

        SDL_RenderPresent(game->renderer);

        float moveSpeed = frameTime * 5.0;
        float rotSpeed = frameTime * 3.0;
        const Uint8 *state = SDL_GetKeyboardState(NULL);

        handleMovement(game, state, moveSpeed, rotSpeed);
    }
}


/**
 * handleMovement - Handles player movement based on input.
 * @game: The game state.
 * @state: The current state of the keyboard.
 * @moveSpeed: The speed of movement.
 * @rotSpeed: The speed of rotation.
 */
void handleMovement(Game *game, const Uint8 *state, float moveSpeed, float rotSpeed) {
    int (*worldMap)[mapWidth][mapHeight] = getWorldMap();
    if (state[SDL_SCANCODE_UP]) {
        if ((*worldMap)[(int)(game->posX + game->dirX * moveSpeed)][(int)(game->posY)] == 0)
            game->posX += game->dirX * moveSpeed;
        if ((*worldMap)[(int)(game->posX)][(int)(game->posY + game->dirY * moveSpeed)] == 0)
            game->posY += game->dirY * moveSpeed;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        if ((*worldMap)[(int)(game->posX - game->dirX * moveSpeed)][(int)(game->posY)] == 0)
            game->posX -= game->dirX * moveSpeed;
        if ((*worldMap)[(int)(game->posX)][(int)(game->posY - game->dirY * moveSpeed)] == 0)
            game->posY -= game->dirY * moveSpeed;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        float oldDirX = game->dirX;
        game->dirX = game->dirX * cos(-rotSpeed) - game->dirY * sin(-rotSpeed);
        game->dirY = oldDirX * sin(-rotSpeed) + game->dirY * cos(-rotSpeed);
        float oldPlaneX = game->planeX;
        game->planeX = game->planeX * cos(-rotSpeed) - game->planeY * sin(-rotSpeed);
        game->planeY = oldPlaneX * sin(-rotSpeed) + game->planeY * cos(-rotSpeed);
    }
    if (state[SDL_SCANCODE_LEFT]) {
        float oldDirX = game->dirX;
        game->dirX = game->dirX * cos(rotSpeed) - game->dirY * sin(rotSpeed);
        game->dirY = oldDirX * sin(rotSpeed) + game->dirY * cos(rotSpeed);
        float oldPlaneX = game->planeX;
        game->planeX = game->planeX * cos(rotSpeed) - game->planeY * sin(rotSpeed);
        game->planeY = oldPlaneX * sin(rotSpeed) + game->planeY * cos(rotSpeed);
    }
}


/**
 * initSDL - Initializes SDL and TTF.
 * @game: The game state to initialize.
 */
void initSDL(Game *game) {
    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    game->font = TTF_OpenFont("assets/fonts/Nunito/static/Nunito-Bold.ttf", 24);
    if (game->font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }

    game->window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, screenWidth,
                                    screenHeight, 0);
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize other game variables
  	game->posX = 22;
    game->posY = 12;
    game->dirX = -1;
    game->dirY = 0;
    game->planeX = 0;
    game->planeY = 0.6;
    game->time = 0;
    game->oldTime = 0;

}


/**
 * cleanup - Cleans up resources before exiting the program.
 * @game: The game state to clean up.
 */
void cleanup(Game *game) {
    TTF_CloseFont(game->font);
    TTF_Quit();
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}
