# Maze 3D Renderer


![Maze 3D Game Screenshot](https://github.com/TheKingsident/Maze-3D/blob/main/assets/maze_3D_screenshot.png)

This project is a simple 3D maze renderer using raycasting techniques similar to early 3D games like Wolfenstein 3D. The application is written in C and utilizes the SDL2 library for rendering. It is solely developed by [Kingsley Usa](https://www.linkedin.com/in/thekingsident/)


Explore the maze in 3D at the deployed site: [Maze3D](https://maze3d.carrd.co/). Stay tuned for an in-depth dive into the development process in the upcoming blog article.

## Introduction

This project aims to recreate the magic of labyrinth exploration with a modern twist. Using the SDL2 library, the Maze 3D Renderer is crafted in C and showcases the power of low-level programming in creating immersive experiences.

Check out the live version [here](https://maze3d.carrd.co/).

Read more about the development journey in the [final project blog article](https://medium.com/@kingsident/journey-through-the-maze-building-my-3d-game-448c8f2ec9e1).

Connect with the me on LinkedIn: [Kingsley Usa](https://www.linkedin.com/in/thekingsident/)

## Features

- A window creation with SDL2
- Raycasting algorithm to draw walls with perspective
- Ability to change the camera angle by modifying the code (orientation feature)
- Walls with different colors based on their orientation (NORTH/SOUTH vs EAST/WEST)
- Camera rotation and movement with keyboard input (rotation and movement feature)
- Collision detection to prevent moving through walls (collision handling feature)
- Map parser to load the maze layout from a file (parser feature)

## Setup

To run this project, you'll need to have SDL2, SDL2_image, and SDL2_ttf installed on your system.

1. Install SDL2: Follow the instructions based on your operating system [here](https://wiki.libsdl.org/Installation).
2. Install SDL2_image: Follow the instructions [here](https://www.libsdl.org/projects/SDL_image/).
3. Install SDL2_ttf: Follow the instructions [here](https://www.libsdl.org/projects/SDL_ttf/).

## Installation

Ensure you have SDL2, SDL2_image, and SDL2_ttf libraries installed to run this project.

```bash
# For Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

# For Fedora
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel

# For Arch Linux
sudo pacman -S sdl2 sdl2_image sdl2_ttf
```

## Compilation

Use the following command to compile the game:

```sh
gcc -o Maze_3D src/*.c -Iheaders -lSDL2 -lSDL2_ttf -lSDL2_image -lm
```

## Running the Game

After compiling, you can run the game with:

```sh
./Maze_3D
```
t
## Controls

- **W/A/S/D**: Move forward/left/back/right
- **Left/Right Arrow**: Rotate camera left/right

## Customizing the Game

The game's source code is structured to allow easy modification and experimentation:

- **`maze_3D.h`**: Contains the constants, type definitions, and function declarations for the game.
- **`game_logic.c`**: Contains the main game loop and rendering logic.

Feel free to dive into the code and tweak values to see how they affect the game!

## Contributions

Contributions to the project are welcome. If you'd like to contribute or suggest improvements, please open an issue or a pull request.

## License

This project is open source and available under the [MIT License](LICENSE).
