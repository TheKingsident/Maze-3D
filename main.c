#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "maze_3D.h"

#define screenWidth 800
#define screenHeight 600



void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
SDL_Color color, int x, int y)
{
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect dstrect = {x, y, surface->w, surface->h};

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}



/**
 * verLine - Draws a vertical line on the renderer.
 *
 * @renderer: SDL_Renderer to draw on.
 * @x: X coordinate of the line.
 * @drawStart: the Y coordinate of the start of the line.
 * @drawEnd: Y coordinate of the end of the line.
 * @color: Color of the line.
 */
void verLine(SDL_Renderer *renderer, int x, int drawStart, int drawEnd,
			 ColorRGB color) {
	if (drawStart < 0)
	{
		drawStart = 0;
	}

	if (drawEnd >= screenHeight)
	{
		drawEnd = screenHeight - 1;
	}

	/* Draw the vertical line*/
	for (int y = drawStart; y <= drawEnd; y++)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
		SDL_RenderDrawPoint(renderer, x, y);
	}
}

/**
 * main - Main function.
 * @argc: Argument counter
 * @argv: argument vector
 * Return: 0 for success, 1 for failure
 */
int main(int argc, char *argv[])
{
	int (*worldMap)[mapWidth][mapHeight] = getWorldMap();
	float posX = 22, posY = 12;          /* x and y start position   */
	float dirX = -1, dirY = 0;           /* initial direction vector */
	float planeX = 0, planeY = 0.6;     /* the 2d raycaster version */
										 /* of camera plane          */

	float time = 0;                      /* time of current frame    */
	float oldTime = 0;                   /* time of previous frame   */

	SDL_Window *window;
	SDL_Renderer *renderer;

	ColorRGB RGB_Red = {255, 0, 0};
	ColorRGB RGB_Green = {0, 255, 0};
	ColorRGB RGB_Blue = {0, 0, 255};
	ColorRGB RGB_White = {255, 255, 255};
	ColorRGB RGB_Yellow = {255, 255, 0};
	ColorRGB RGB_Grey = {128, 128, 128};

	SDL_Init(SDL_INIT_VIDEO);

	if (TTF_Init() == -1)
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	TTF_Font *font = TTF_OpenFont(
		"assets/fonts/Nunito/static/Nunito-Bold.ttf", 24);

	if (font == NULL)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}



	window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED, screenWidth,
							  screenHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	while (1)
	{
		SDL_Event e;

		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				break;
		}

		/* Clear screen */
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (int x = 0; x < screenWidth; x++)
		{
			/* Calculate ray position and direction */
			float cameraX = 2 * x / (float)screenWidth - 1;
			float rayDirX = dirX + planeX * cameraX;
			float rayDirY = dirY + planeY * cameraX;

			int mapX = (int)(posX);
			int mapY = (int)(posY);

			/** length of ray from current position to next x or y-side */
			float sideDistX;
			float sideDistY;

			/**
			 * these are derived as:
			 * deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
			 * deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
			 * which can be simplified to abs(|rayDir| / rayDirX)
			 * and abs(|rayDir| / rayDirY)
			 * where |rayDir| is the length of the vector (rayDirX, rayDirY).
			 * Its length,
			 * unlike (dirX, dirY) is not 1, however this does not matter, only the
			 * ratio between deltaDistX and deltaDistY matters, due to the way the DDA
			 * stepping further below works. So the values can be computed as below.
			 * Division through zero is prevented, even though technically that's not
			 * needed in C++ with IEEE 754 floating point values.
			 */

			float deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			float deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));

			float perpWallDist;

			/* what direction to step in x or y-direction (either +1 or -1) */
			int stepX;
			int stepY;

			int hit = 0; /* was there a wall hit? */
			int side;    /* was a NS or a EW wall hit? */
			/* calculate step and initial sideDist */
			if (rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (posX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - posX) * deltaDistX;
			}
			if (rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (posY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - posY) * deltaDistY;
			}
			/* perform DDA */
			while (hit == 0)
			{
				/* jump to next map square, either in x-direction, or in y-direction */
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				/* Check if ray has hit a wall */
				if ((*worldMap)[mapX][mapY] > 0)
					hit = 1;
			}

			/**
			 * Calculate distance projected on camera direction.
			 * This is the shortest distance from the point where the wall is
			 * hit to the camera plane. Euclidean to center camera point would
			 * give fisheye effect!
			 * This can be computed as (mapX - posX + (1 - stepX) / 2) /
			 * rayDirX for side == 0, or same formula with Y
			 * for size == 1, but can be simplified to the code below thanks to
			 * how sideDist and deltaDist are computed:
			 * because they were left scaled to |rayDir|. sideDist is the entire
			 * length of the ray above after the multiple
			 * steps, but we subtract deltaDist once because one step more into
			 * the wall was taken above.
			 */
			if (side == 0)
				perpWallDist = fabs((mapX - posX + (1 - stepX) / 2) / rayDirX);
			else
				perpWallDist = fabs((mapY - posY + (1 - stepY) / 2) / rayDirY);

			/* Calculate height of line to draw on screen */
			int h = screenHeight;
			int lineHeight = (int)(3 * h / perpWallDist);

			/* calculate lowest and highest pixel to fill in current stripe */
			int drawStart = -lineHeight / 2 + h / 2;

			if (drawStart < 0)
				drawStart = 0;
			int drawEnd = lineHeight / 2 + h / 2;

			if (drawEnd >= h)
				drawEnd = h - 1;

			/* choose wall color */
			ColorRGB color;

			switch ((*worldMap)[mapX][mapY])
			{
			case 1:
				color = RGB_Red;
				break; /* red */
			case 2:
				color = RGB_Green;
				break; /* green */
			case 3:
				color = RGB_Blue;
				break; /* blue */
			case 4:
				color = RGB_White;
				break; /* white */
			default:
				color = RGB_Yellow;
				break; /* yellow */
			}

			/* give x and y sides different brightness */
			/* give x and y sides different brightness */
			if (side == 1)
			{
				color.r /= 2;
				color.g /= 2;
				color.b /= 2;
			}

			/* draw the pixels of the stripe as a vertical line */
			verLine(renderer, x, drawStart, drawEnd, color);

			/* Draw vertical line */
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
			SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
		}



		/* Timing for input and FPS counter */
		oldTime = time;
		time = SDL_GetTicks();
		float frameTime = (time - oldTime) / 1000.0; /* frameTime is the time*/
													  /* this frame has taken, in seconds */

		char fpsText[16];

		sprintf(fpsText, "FPS: %.2f", 1.0 / frameTime);
		SDL_Color textColor = {255, 255, 255, 255};

		renderText(renderer, font, fpsText, textColor, 10, 10);

		/* Present renderer */
		SDL_RenderPresent(renderer);

		/* Speed modifiers */
		float moveSpeed = frameTime * 5.0; /* the constant value is in squares */
											/* /second */
		float rotSpeed = frameTime * 3.0;  /* the constant value is*/
											/* in radians/second */

		/* Read keys and update position and direction */
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		if (state[SDL_SCANCODE_UP])
		{
			/* Move forward if no wall in front of you */
			if ((*worldMap)[(int)(posX + dirX * moveSpeed)][(int)(posY)] == 0)
				posX += dirX * moveSpeed;
			if ((*worldMap)[(int)(posX)][(int)(posY + dirY * moveSpeed)] == 0)
				posY += dirY * moveSpeed;
		}
		if (state[SDL_SCANCODE_DOWN])
		{
			/* Move backwards if no wall behind you */
			if ((*worldMap)[(int)(posX - dirX * moveSpeed)][(int)(posY)] == 0)
				posX -= dirX * moveSpeed;
			if ((*worldMap)[(int)(posX)][(int)(posY - dirY * moveSpeed)] == 0)
				posY -= dirY * moveSpeed;
		}
		if (state[SDL_SCANCODE_RIGHT])
		{
			/* Rotate to the right */
			float oldDirX = dirX;

			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			float oldPlaneX = planeX;

			planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
			planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		if (state[SDL_SCANCODE_LEFT])
		{
			/* Rotate to the left */
			float oldDirX = dirX;

			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			float oldPlaneX = planeX;

			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}
	}

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return (0);
}
