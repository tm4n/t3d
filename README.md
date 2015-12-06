# t3d

This engine is being written for the rewrite of the [Survive!](http://www.indiedb.com/games/survive) video game.
It's very simple and doesn't offer advanced scene management or much customization in general. All is tailored for a small and simple 3d world. Compatible asset formats are very limited.


### Features:
 - Should work on Windows, Linux and Android (Ouya tested), based on SDL
 - OpenGL 2.1 and ES 2.0, targeted to work on older intel gmas
 - KISS principle
 - Simple gui and text rendering
 - Simple particle system
 - Per-object vertex and pixel shaders supported
 - Few dependencies, own loaders for assets 

### Missing, will probably be added later on:
 - depth sorting for transparent objects
 - library build system
 - example build for linux + android
 - particle system performance improvements
 - make normals available for shaders
 - post-processing shaders, shadow system
 - simulated 3D sound effects (basic code is already there, but commented out)

### Dependencies:
 - [SDL2](https://www.libsdl.org/)
 - [SDL2-mixer](https://www.libsdl.org/projects/SDL_mixer/)
 - [SDL2-ttf](https://www.libsdl.org/projects/SDL_ttf/)
 - [glm](http://glm.g-truc.net)

### Asset formats
 - TGA for images (only uncompressed and 24/32 bits)
 - [MDL5](http://www.conitec.net/manual_d/prog_mdlhmp.html) for 3d models (supporting frame animations)
 - ttf for fonts
 - wav for sound, though others could easily be supported through SDL-mixer 
