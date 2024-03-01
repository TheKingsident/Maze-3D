#ifndef MAZE_3D_H
#define MAZE_3D_H

#define mapWidth 24
#define mapHeight 24


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

#endif /* MAZE_3D_H */
