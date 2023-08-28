![demo screenshot 1](https://github.com/Augusto-Concordia/underwater_bonanza/assets/33916325/4011ee48-ccd9-44e8-a181-568676c1d79d)

# Pretty Blue (underwater_bonanza)
Final Project - COMP 371 - Team 18

- Noor Hammodi
- Nicolas Towa Kamgne
- Gabrielle Guidote
- Jack Spiratos
- Augusto Mota Pinheiro

## Goals
The goal of this project is to create a graphics program using OpenGL to enable walking through a procedurally created virtual world. Our virtual world starts above sea level, and as the user moves, they are introduced to the depths of the ocean. There, one can see and navigate through sea life, including fish, sharks, seaweeds, rocks, caves, and more. This terrain is procedurally created.

## Features
* Procedural World Generation (Using marching cubes)
* Procedural Foliage Generation
* Random Foliage Placement
* Procedure Foliage Animation
* Procedural Fish Generation with Procedural Animation
* Fish AI for Navigation
* Volumetric Lighting
* Water Caustic Effect
* Procedural Ocean Surface
* Story Cutscene Featuring Original Assets

## Demo
### Video Overview
[![Thumbnail of Project Trailer](https://img.youtube.com/vi/eNTy1FF-BFI/0.jpg)](https://www.youtube.com/watch?v=eNTy1FF-BFI)

### Screenshots
![demo screenshot 2](https://github.com/Augusto-Concordia/underwater_bonanza/assets/33916325/d5f0d6d8-2c09-46cd-86fb-cf5f2d805224)
![demo screenshot 3](https://github.com/Augusto-Concordia/underwater_bonanza/assets/33916325/7e62d8e7-650c-420f-b0bb-6a74ccaf4ba1)
![demo screenshot 4](https://github.com/Augusto-Concordia/underwater_bonanza/assets/33916325/46ab37be-661a-4641-beec-634deb977621)


## Getting Started
Make sure that you clone the repository with `--recurse-submodules` to include the depended upon libraries.

1. Generate project files with CMake
2. Set the working directory to the root of the project
3. Run the `underwater_bonanza` project!

## Keybinds
* `WASD`: moves the camera
* `Mouse`: looks around
* `Esc`: quits
* `Space`: restarts cutscene and generation
* `Tab`: skip cutscene and regenerate
* `X`: unlocks freecam (over water scene only)

## Attributions
* Rendering using OpenGL
* [`glfw`](https://www.glfw.org/) for Window Handling
* Loading model using [`assimp`](https://www.assimp.org/)
* [`stb_image`](https://github.com/nothings/stb) for Texture Loading
* [`SFML`](https://www.sfml-dev.org/) for Sound Playback
* [Learn OpenGL](https://learnopengl.com/) for Basic Concepts
* Caustic Textures Generated with [Free Caustics Generator](https://www.cathalmcnally.com/news/free-caustics-generator/)


