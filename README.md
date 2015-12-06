# t3d

This engine is being written for the rewrite of the [Survive!](http://www.indiedb.com/games/survive) video game.
It's very simple and doesn't offer advanced scene management or much customization in general. All is tailored for a small and simple 3d world. Compatible asset formats are very limited.

Should work on Windows, Linux and Android.

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
