#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "maze_3D.h"


/**
 * castRay - Casts a ray from the player's position in the direction of the
 * camera plane.
 *
 * @game: The game structure containig the players position and direction.
 * @cameraX: The x-coordinte on the camera plane where the ray intersects.
 * Return: A RaycastResult structure containing information about the raycast.
 */
RaycastResult castRay(Game *game, float cameraX)
{
	RaycastResult result;
	float rayDirX;
	float rayDirY;
	float sideDistX;
	float sideDistY;
	float deltaDistX;
	float deltaDistY;
	int stepX;
	int stepY;
	int mapX;
	int mapY;
	int side;
	int hit;

	/* Calculate the direction of the ray */
	rayDirX = game->dirX + game->planeX * cameraX;
	rayDirY = game->dirY + game->planeY * cameraX;

	/* Get the player's position in the map */
	mapX = (int)(game->posX);
	mapY = (int)(game->posY);

	/* Calculate the length of the ray from one side to the next in the map */
	deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
	deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));

	/* Initialize the hit flag to 0 */
	hit = 0;

	/* Determine the step direction and initial side distance */
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
	if (rayDirY < 0)
	{
		stepY = -1;
		sideDistY = (game->posY - mapY) * deltaDistY;
	}
	else
	{
		stepY = 1;
		sideDistY = (mapY + 1.0 - game->posY) * deltaDistY;
	}

	/* Perform DDA algorithm to find the wall the ray hits */
	while (hit == 0)
	{
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
		if ((*getWorldMap())[mapX][mapY] > 0)
			hit = 1;
	}

	/* Calculate the perpendicular distance to the wall*/
	if (side == 0)
		result.perpWallDist = fabs((mapX - game->posX + (
			1 - stepX) / 2) / rayDirX);
	else
		result.perpWallDist = fabs((mapY - game->posY + (
			1 - stepY) / 2) / rayDirY);

	/* Store the results in the RaycastResult structure*/
	result.side = side;
	result.mapX = mapX;
	result.mapY = mapY;
	result.rayDirX = rayDirX;
	result.rayDirY = rayDirY;

	/* Return the result */
	return (result);
}


/**
 * drawWall - Draws a wall column based on the raycast result.
 *
 * @game: The game structure containing rendering information.
 * @x: The x-coordinate of the column to draw on the screen.
 * @raycastResult: result containing information about the wall hit.
 */
void drawWall(Game *game, int x, RaycastResult raycastResult)
{
	int (*worldMap)[mapWidth][mapHeight];
	SDL_Rect srcRect;
	SDL_Rect destRect;
	int lineHeight;
	int texNum;
	int drawStart;
	int drawEnd;
	int texX;
	int texStart;
	float wallX;
	float texStep;

	/* Get the world map */
	worldMap = getWorldMap();

	/* Calculate the height of the wall column */
	lineHeight = (int)(screenHeight / raycastResult.perpWallDist);
	drawStart = -lineHeight / 2 + screenHeight / 2;

	/* Ensure the drawing start and end positions are within the screen bounds */
	if (drawStart < 0)
		drawStart = 0;
	drawEnd = lineHeight / 2 + screenHeight / 2;
	if (drawEnd >= screenHeight)
		drawEnd = screenHeight - 1;

	/* Get the texture number and calculate the x-coordinate on the texture */
	/* Assuming textures start from 1 in the map */
	texNum = (*worldMap)[raycastResult.mapX][raycastResult.mapY] - 1;
	wallX = (raycastResult.side == 0)
			? game->posY + raycastResult.perpWallDist * raycastResult.rayDirY
			: game->posX + raycastResult.perpWallDist * raycastResult.rayDirX;
	wallX -= floor(wallX);
	texX = (int)(wallX * (float)TEX_WIDTH);

	/* Adjust texture x-coordinate for the wall's side and direction */
	if ((raycastResult.side == 0 && raycastResult.rayDirX > 0) ||
		(raycastResult.side == 1 && raycastResult.rayDirY < 0))
	{
		texX = TEX_WIDTH - texX - 1;
	}

	/* Calculate the texture step and start position */
	texStep = (float)TEX_HEIGHT / lineHeight;
	texStart = (drawStart - (screenHeight - lineHeight) / 2) * texStep;

	if (texStart < 0)
	{
		texStart = 0;
	}

	/* Set up the source and destination rectangles for rendering */
	srcRect.x = texX;
	srcRect.y = texStart;
	srcRect.w = 1; /* We're only drawing a single column of the texture */
	srcRect.h = TEX_HEIGHT - texStart;

	destRect.x = x;
	destRect.y = drawStart;
	destRect.w = 1; /* Corresponds to the width of the source rectangle */
	destRect.h = drawEnd - drawStart;

	/* Update the draw end position in the game structure */
	game->drawEnd = drawEnd;

	if (drawEnd < game->drawEnd)
	{
		game->drawEnd = drawEnd;

	}

	/* Render the texture or a fallback color */
	if (texNum >= 0 && texNum < NUM_TEXTURES)
	{
		if (raycastResult.side == 1)
		{
			/* Darken the texture for side walls */
			SDL_SetTextureColorMod(game->textures[texNum], 128, 128, 128);
		}
		else
		{
			/* Full brightness for front walls */
			SDL_SetTextureColorMod(game->textures[texNum], 255, 255, 255);
		}
		SDL_RenderCopy(game->renderer, game->textures[texNum], &srcRect, &destRect);
	}
	else
	{
		/* Fallback to a solid color if the texture index is out of range */
		ColorRGB color = {255, 0, 0}; /* Red, for example */

		SDL_SetRenderDrawColor(game->renderer, color.r, color.g, color.b, 255);
		SDL_RenderDrawLine(game->renderer, x, drawStart, x, drawEnd);
	}
}


/**
 * renderScene - Renders the scene by casting rays and drawing walls
 * for each column on the screen.
 *
 * @game: The game structure containing rendering and game state
 * information.
 */
void renderScene(Game *game)
{
	/* Iterate over each column on the screen */
	for (int x = 0; x < screenWidth; x++)
	{
		/* Calculate the x-coordinate on the camera plane */
		float cameraX = 2 * x / (float)screenWidth - 1;

		/* Cast a ray and get the result */
		RaycastResult raycastResult = castRay(game, cameraX);

		/* Draw the wall based on the raycast result */
		drawWall(game, x, raycastResult);
	}

}


/**
 * renderFPS - Renders the frames per second (FPS) on the screen.
 *
 * @game: The game structure containing rendering information.
 * @frameTime: The time taken to render the last frame, in seconds.
 */
void renderFPS(Game *game, float frameTime)
{
	/* Prepare the FPS text */
	char fpsText[16];

	sprintf(fpsText, "FPS: %.2f", 1.0 / frameTime);

	/* Set the text color to white */
	SDL_Color textColor = {255, 255, 255, 255};

	/* Render the text on the screen at position (10, 10) */
	renderText(game->renderer, game->font, fpsText, textColor, 10, 10);
}
