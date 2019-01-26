#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SDL/SDL.h>

///////////////////////////////////////////////////////////////////////////////
#define WIDTH 800
#define HEIGHT 600
#define DELAY 40
#define APPLES_NUM 50
#define MAX_TIME_INTERVAL 5
#define BG_IMAGE "BG.bmp"
#define DBG_FLAG FALSE

///////////////////////////////////////////////////////////////////////////////
typedef enum BOOL
{
    FALSE = 0,
    TRUE  = 1,
} BOOL;

typedef enum Direct
{
    RIGHT =  1,
    LEFT  = -1,
} Direct;

///////////////////////////////////////////////////////////////////////////////
typedef struct HitBox
{
    SDL_Rect body;
    Uint32 color;   //for debug
	BOOL borderDraw;//for debug
} HitBox;

typedef struct Hedgehog
{
    SDL_Rect rect;
    Direct direct;
    Sint16 speed;
    char image[256];
    HitBox hitbox;
} Hedgehog;

typedef struct Stick
{
    Sint16 x1, y1;
    Sint16 x2, y2;
    Uint32 color;
} Stick;

typedef struct Apple
{
    Sint16 x0, y0;
    Uint16 radius;
    Uint32 color;
    Stick stick;
    BOOL inFalling;
    Sint16 fallSpeed;
    HitBox hitbox;
} Apple;

///////////////////////////////////////////////////////////////////////////////
void InitHedgehog();
void InitApples();
void DrawBackground(SDL_Surface *surf);
void DrawHedgehog(SDL_Surface *surf);
void DrawApple(SDL_Surface *surf, int num);
void DrawFrame(SDL_Surface *surf);
void ChangeAppleState(int num);
BOOL CheckCollision(HitBox apple, HitBox hedgehog);
BOOL FindCollision();
BOOL RandTimeInterval();
BOOL StartingFall();

#endif //HEDGEHOG_H