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
	/* Continue the loop until a quit event is received */
	while (1)
	{
		SDL_Event e;
		/* Poll for events and handle the quit event*/
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				break;
		}

		/* Set the background color and clear the screen */
		SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 100);
		SDL_RenderClear(game->renderer);

		/* Render the scene */
		renderScene(game);

		/* Calculate the time taken for the last frame */
		game->oldTime = game->time;
		game->time = SDL_GetTicks();
		float frameTime = (game->time - game->oldTime) / 1000.0;

		/* Render the frames per second (FPS) */
		renderFPS(game, frameTime);

		/* Present the rendered frame */
		SDL_RenderPresent(game->renderer);

		/* Calculate movement and rotation speeds based on frame time */
		float moveSpeed = frameTime * 5.0;
		float rotSpeed = frameTime * 3.0;

		/* Get the current state of the keyboard */
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		/* Handle player movement based on keyboard input */
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
void handleMovement(Game *game, const Uint8 *state,
					float moveSpeed, float rotSpeed)
{
	/* Get the world map */
	int (*worldMap)[mapWidth][mapHeight] = getWorldMap();

	/* Move forward */
	if (state[SDL_SCANCODE_UP])
	{
		/* Check if the new position is walkable and update the position */
		if ((*worldMap)[(int)(
			game->posX + game->dirX * moveSpeed)][(int)(game->posY)] == 0)
			game->posX += game->dirX * moveSpeed;
		if ((*worldMap)[(int)(
			game->posX)][(int)(game->posY + game->dirY * moveSpeed)] == 0)
			game->posY += game->dirY * moveSpeed;
	}

	/* Move backward */
	if (state[SDL_SCANCODE_DOWN])
	{
		/* Check if the new position is walkable and update the position */
		if ((*worldMap)[(int)(
			game->posX - game->dirX * moveSpeed)][(int)(game->posY)] == 0)
			game->posX -= game->dirX * moveSpeed;
		if ((*worldMap)[(int)(game->posX)][(int)(
			game->posY - game->dirY * moveSpeed)] == 0)
			game->posY -= game->dirY * moveSpeed;
	}

	/* Rotate right */
	if (state[SDL_SCANCODE_RIGHT])
	{

		/* Store the old direction */
		float oldDirX = game->dirX;

		/* Calculate the new direction using rotation matrix */
		game->dirX = game->dirX * cos(-rotSpeed) - game->dirY * sin(-rotSpeed);
		game->dirY = oldDirX * sin(-rotSpeed) + game->dirY * cos(-rotSpeed);

		/* Store the old plane */
		float oldPlaneX = game->planeX;

		/* Calculate the new plane using rotation matrix */
		game->planeX = game->planeX * cos(-rotSpeed) - game->planeY * sin(-rotSpeed);
		game->planeY = oldPlaneX * sin(-rotSpeed) + game->planeY * cos(-rotSpeed);
	}

	/* Rotate left */
	if (state[SDL_SCANCODE_LEFT])
	{
		/* Store the old direction */
		float oldDirX = game->dirX;

		/* Calculate the new direction using rotation matrix */
		game->dirX = game->dirX * cos(rotSpeed) - game->dirY * sin(rotSpeed);
		game->dirY = oldDirX * sin(rotSpeed) + game->dirY * cos(rotSpeed);

		/* Store the old plane */
		float oldPlaneX = game->planeX;

		/* Calculate the new plane using rotation matrix */
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

	if (TTF_Init() == -1)
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	game->font = TTF_OpenFont("assets/fonts/Nunito/static/Nunito-Bold.ttf", 24);
	if (game->font == NULL)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}

	game->window = SDL_CreateWindow("Maze 3D", SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, screenWidth,
									screenHeight, 0);
	game->renderer = SDL_CreateRenderer(game->window, -1,
										SDL_RENDERER_ACCELERATED);

	game->posX = 14;
	game->posY = 12;
	game->dirX = -1;
	game->dirY = 0;
	game->planeX = 0;
	game->planeY = 0.6;
	game->time = 0;
	game->oldTime = 0;

	const char *textureFiles[NUM_TEXTURES] = {
		"assets/textures/purplestone.png",
		"assets/textures/colorstone.png",
		"assets/textures/eagle.png",
		"assets/textures/greystone.png",
		"assets/textures/mossy.png",
		"assets/textures/bluestone.png",
		"assets/textures/redbrick.png",
		"assets/textures/wood.png",
		"assets/textures/wooden_1.jpg"
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
		}
		game->textures[i] =  SDL_CreateTextureFromSurface(game->renderer,
															tempSurface);

		SDL_FreeSurface(tempSurface);
	}

	game->floorTexture = loadStreamingTexture("assets/textures/colorstone.png",
												game->renderer);
	if (!game->floorTexture)
	{
		fprintf(stderr, "Failed to load floor texture\n");
	}


}

SDL_Texture *loadStreamingTexture(const char *filePath, SDL_Renderer *renderer)
{
	SDL_Surface *surface = IMG_Load(filePath);

	if (!surface)
	{
		fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
		return (NULL);
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, surface->format->format,
												SDL_TEXTUREACCESS_STREAMING,
												surface->w, surface->h);

	if (!texture)
	{
		fprintf(stderr, "Failed to create streaming texture: %s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		return (NULL);
	}

	SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
	SDL_FreeSurface(surface);
	return (texture);
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
