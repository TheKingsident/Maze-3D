#include "maze_3D.h"


/**
 * main - Entry point of the program.
 * @argc: Argument count.
 * @argv: Argument vector.
 * Return: 0 on success, non-zero on failure.
 */
int main(int argc, char *argv[])
{
	Game game = {0};

	initSDL(&game);
	gameLoop(&game);
	cleanup(&game);
	return (0);
}
