#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
		SDL_SetRenderDrawColor(game->renderer, 135, 206, 235, 255);
		SDL_RenderClear(game->renderer);

        SDL_Rect srcRect, destRect;


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

            float hitX;
            if (side == 0) {
                hitX = game->posY + perpWallDist * rayDirY;
            } else {
                hitX = game->posX + perpWallDist * rayDirX;
            }
            hitX -= floor(hitX);  // Normalize to a value between 0 and 1

            int h = screenHeight;
            int lineHeight = (int)(h / perpWallDist);

            int drawStart = -lineHeight / 2 + h / 2;
            if (drawStart < 0)
                drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if (drawEnd >= h)
                drawEnd = h - 1;

            int texNum = (*worldMap)[mapX][mapY] - 1; // Assuming your textures start from 1 in the map
            float wallX = side == 0 ? game->posY + perpWallDist * rayDirY : game->posX + perpWallDist * rayDirX;
            wallX -= floor(wallX);

            int texX = (int)(wallX * (float)TEX_WIDTH);
            if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0)) {
                texX = TEX_WIDTH - texX - 1;
            }

            float texStep = (float)TEX_HEIGHT / lineHeight;
            int texStart = (drawStart - (screenHeight - lineHeight) / 2) * texStep;
            if (texStart < 0)
            {
                texStart = 0;
            }

            srcRect.x = texX;
            srcRect.y = texStart;
            srcRect.w = 1; // We're only drawing a single column of the texture
            srcRect.h = TEX_HEIGHT - texStart;

            destRect.x = x;
            destRect.y = drawStart;
            destRect.w = 1; // Corresponds to the width of the source rectangle
            destRect.h = drawEnd - drawStart;

            game->drawEnd = drawEnd;

			if (drawEnd < game->drawEnd)
			{
				game->drawEnd = drawEnd;

			}

            if (texNum >= 0 && texNum < NUM_TEXTURES)
            {
                if (side == 1)
                {
                    SDL_SetTextureColorMod(game->textures[texNum], 128, 128, 128);
                }
                else
                {
                    SDL_SetTextureColorMod(game->textures[texNum], 255, 255, 255);
                }
                SDL_RenderCopy(game->renderer, game->textures[texNum], &srcRect, &destRect);
            } else {
                // Fallback to a solid color if the texture index is out of range
                ColorRGB color = {255, 0, 0}; // Red, for example
                SDL_SetRenderDrawColor(game->renderer, color.r, color.g, color.b, 255);
                SDL_RenderDrawLine(game->renderer, x, drawStart, x, drawEnd);
            }
            

        }		

        int pitch;

        Uint32 *floorPixels = lockTextureAndGetPixels(game->floorTexture, &pitch);
        if (!floorPixels) {
            printf("Unable to retreive floor pixels");
        }

		for (int y = game->drawEnd + 1; y < screenHeight; ++y)
		{
			SDL_Rect srcRect;
    		SDL_Rect destRect = {0, y, screenWidth, 1}; // Stretch the srcRect to the entire width of the screen

			// Calculate the current position on the floor
			float rayDirX0 = game->dirX - game->planeX;
			float rayDirY0 = game->dirY - game->planeY;
			float rayDirX1 = game->dirX + game->planeX;
			float rayDirY1 = game->dirY + game->planeY;

			int p = y - screenHeight / 2;
			float posZ = 0.5 * screenHeight;
			float rowDistance = posZ / p;

			float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
			float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;
			float floorX = game->posX + rowDistance * rayDirX0;
			float floorY = game->posY + rowDistance * rayDirY0;

			float leftFloorX = game->posX + rowDistance * rayDirX0;
			float leftFloorY = game->posY + rowDistance * rayDirY0;
			float rightFloorX = game->posX + rowDistance * rayDirX1;
			float rightFloorY = game->posY + rowDistance * rayDirY1;

			int leftTexX = (int)(TEX_WIDTH * (leftFloorX - (int)leftFloorX)) & (TEX_WIDTH - 1);
			int leftTexY = (int)(TEX_HEIGHT * (leftFloorY - (int)leftFloorY)) & (TEX_HEIGHT - 1);

			int rightTexX = (int)(TEX_WIDTH * (rightFloorX - (int)rightFloorX)) & (TEX_WIDTH - 1);

			// Set up the source rectangle for texture sampling
			srcRect.x = leftTexX;
			srcRect.y = leftTexY;
			srcRect.w = rightTexX - leftTexX; // You may need to adjust this depending on your setup
			srcRect.h = 1; // We're drawing one row at a time

			// Now render the floor texture row
			// Make sure the floor texture is the one you want to use for the floor
			SDL_RenderCopy(game->renderer, game->floorTexture, &srcRect, &destRect);

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

void put_pixel(SDL_Renderer* renderer, int x, int y, Uint32 color) {

	Uint8 r = (color >> 24) & 0xFF;
    Uint8 g = (color >> 16) & 0xFF;
    Uint8 b = (color >> 8) & 0xFF;
    Uint8 a = color & 0xFF;  // If your color is in 0xAARRGGBB format

	printf("Drawing pixel with color RGBA(%d, %d, %d, %d)\n", r, g, b, a);
    // Set the draw color to the pixel color
    SDL_SetRenderDrawColor(renderer, r, g, b, a);


    // Draw the pixel
    SDL_RenderDrawPoint(renderer, x, y);
}



Uint32 *lockTextureAndGetPixels(SDL_Texture *texture, int *pitch) {
    Uint32 *pixels = NULL;
    if (SDL_LockTexture(texture, NULL, (void **)&pixels, pitch) < 0) {
        fprintf(stderr, "SDL_LockTexture failed: %s\n", SDL_GetError());
        return NULL;
    }
    return pixels;
}



void unlockTexture(SDL_Texture *texture) {
    SDL_UnlockTexture(texture);
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
void initSDL(Game *game)
{
    SDL_Surface *tempSurface;
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

    const char *textureFiles[NUM_TEXTURES] = {
        "assets/textures/fencing.jpg",
        "assets/textures/marble1.jpg",
        "assets/textures/marble2.jpg",
        "assets/textures/marble3.jpg",
        "assets/textures/marble4.jpg",
        "assets/textures/marble5.jpg",
        "assets/textures/white_marble_texture.jpg",
        "assets/textures/wooden_1.jpg",
		"assets/textures/colorstone.png"
    };

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "anisotropic"))
    {
        printf("Warning: Linear texture filtering not enabled!");
    }

    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        tempSurface = IMG_Load(textureFiles[i]);
        if (!tempSurface)
        {
            printf("Failed to load texture: %s\n", IMG_GetError());
            // Handle error (e.g., exit the program)
        }
        game->textures[i] =  SDL_CreateTextureFromSurface(game->renderer, tempSurface);
        
        
        SDL_FreeSurface(tempSurface);
    }

	game->floorTexture = loadStreamingTexture("assets/textures/white_marble_texture.jpg", game->renderer);
	if (!game->floorTexture) {
		fprintf(stderr, "Failed to load floor texture\n");
		// Handle error
	}


}

SDL_Texture* loadStreamingTexture(const char* filePath, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, surface->format->format,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             surface->w, surface->h);

    if (!texture) {
        fprintf(stderr, "Failed to create streaming texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_FreeSurface(surface);
    return texture;
}



/**
 * cleanup - Cleans up resources before exiting the program.
 * @game: The game state to clean up.
 */
void cleanup(Game *game)
{
    TTF_CloseFont(game->font);
    TTF_Quit();
	SDL_DestroyTexture(game->floorTexture);

    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        SDL_DestroyTexture(game->textures[i]);
    }

    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}
