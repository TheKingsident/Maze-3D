#ifndef MAZE_3D_H
#define MAZE_3D_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define mapWidth 24
#define mapHeight 24
#define TEX_HEIGHT 64
#define TEX_WIDTH 64
#define screenWidth 1080
#define screenHeight 720
#define NUM_TEXTURES 9


int (*getWorldMap(void))[mapWidth][mapHeight];


/**
 * struct ColorRGB - Represents an RGB color.
 * @r: The red component of the color.
 * @g: The green component of the color.
 * @b: The blue component of the color.
 *
 * This structure is used to represent a color in RGB format,
 * where each component is an integer value ranging from 0 to 255.
 */
typedef struct ColorRGB
{
	int r;
	int g;
	int b;
} ColorRGB;


/**
 * struct RaycastResult - Structure to store the result of a raycast operation.
 *
 * @perpWallDist: The perpendicular distance from the player's position
 * to the wall hit by the ray.
 * @side: Indicates which side of the wall was hit by the ray
 * (0 for X-axis, 1 for Y-axis).
 * @mapX: The X-coordinate in the map where the wall was hit.
 * @mapY: The Y-coordinate in the map where the wall was hit.
 * @rayDirX: The X-component of the ray's direction.
 * @rayDirY: The Y-component of the ray's direction.
 */
typedef struct RaycastResult
{
	float perpWallDist;
	int side;
	int mapX;
	int mapY;
	float rayDirX;
	float rayDirY;
} RaycastResult;


/**
 * struct Game - Holds the state of the game.
 * @window: SDL window.
 * @renderer: SDL renderer.
 * @font: TTF font.
 * @posX: X position of the player.
 * @posY: Y position of the player.
 * @dirX: X direction of the player.
 * @dirY: Y direction of the player.
 * @planeX: X component of the camera plane.
 * @planeY: Y component of the camera plane.
 * @time: Current frame time.
 * @oldTime: Previous frame time.
 * @textures: Texture to apply.
 * @floorTexture: Floor texture
 * @drawEnd: Y-coordinate of the end of the wall slice
 */
typedef struct Game
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	float posX, posY;
	float dirX, dirY;
	float planeX, planeY;
	float time, oldTime;
	SDL_Texture * textures[NUM_TEXTURES];
	SDL_Texture *floorTexture;
	int drawEnd;
} Game;


void initSDL(Game *game);
void gameLoop(Game *game);
void renderScene(Game *game);
void renderFPS(Game *game, float frameTime);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
					SDL_Color color, int x, int y);
void verLine(SDL_Renderer *renderer, int x, int drawStart, int drawEnd,
					ColorRGB color);
void handleMovement(Game *game, const Uint8 *state, float moveSpeed,
						float rotSpeed);
SDL_Texture *loadStreamingTexture(const char *filePath,
									SDL_Renderer *renderer);
void cleanup(Game *game);

#endif /* MAZE_3D_H */
