#ifndef MAZE_3D_H
#define MAZE_3D_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define mapWidth 24
#define mapHeight 24
#define TEX_HEIGHT 256
#define TEX_WIDTH 256
#define screenWidth 800
#define screenHeight 600
#define NUM_TEXTURES 8


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
	SDL_Texture *textures[NUM_TEXTURES];
} Game;



extern ColorRGB RGB_Red;
extern ColorRGB RGB_Green;
extern ColorRGB RGB_Blue;
extern ColorRGB RGB_White;
extern ColorRGB RGB_Yellow;
extern ColorRGB RGB_Grey;

void initSDL(Game *game);
void gameLoop(Game *game);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
SDL_Color color, int x, int y);
void verLine(SDL_Renderer *renderer, int x, int drawStart, int drawEnd,
ColorRGB color);
void handleMovement(Game *game, const Uint8 *state, float moveSpeed,
float rotSpeed);
void cleanup(Game *game);

#endif /* MAZE_3D_H */
