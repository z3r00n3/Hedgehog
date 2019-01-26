#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include "hedgehog.h"

///////////////////////////////////////////////////////////////////////////////
extern BOOL StartFall;
extern BOOL TheEnd;

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Error SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_ANYFORMAT);
    if(!screen)
    {
        fprintf(stderr, "SDL mode failed: %s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL));
	InitHedgehog();
    InitApples();

    for( ; ; )
    {
        if(FindCollision()) TheEnd = TRUE;
        DrawFrame(screen);
        SDL_Delay(DELAY);

        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
                StartFall = TRUE;

            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                SDL_Quit();
                return 0;
            }
        }
    }

    fprintf(stderr, "WaitEvent failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}




