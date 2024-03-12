#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "maze_3D.h"


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

void verLine(SDL_Renderer *renderer, int x, int drawStart, int drawEnd,
ColorRGB color)
{
	if (drawStart < 0)
	{
		drawStart = 0;
	}
	if (drawEnd >= screenHeight)
	{
		drawEnd = screenHeight - 1;
	}
	for (int y = drawStart; y <= drawEnd; y++)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
		SDL_RenderDrawPoint(renderer, x, y);
	}
}
