#include <SDL/SDL.h>
#include <SDL/SDL_draw.h>
#include <time.h>
#include "hedgehog.h"

///////////////////////////////////////////////////////////////////////////////
Uint32 Colors[] =
{
    0xd41f0d, //red
    0xf8132e, //red
    0xec2f2f, //red
    0xf91d2e, //red
    0xfbfb00, //yellow
    0xd0e906, //yellow
    0xe1ee0b, //yellow
    0xfdf200, //yellow
    0x00ff04, //green
    0x12d43a, //green
    0x6eec2f, //green
    0x4bb712, //green
};

Apple Apples[APPLES_NUM];
Hedgehog HedgeHog;
BOOL StartFall = FALSE;
BOOL TheEnd = FALSE;

///////////////////////////////////////////////////////////////////////////////
void InitHedgehog()
{
    HedgeHog.rect.y = 490;
	HedgeHog.direct = RIGHT;
    HedgeHog.speed  = 3;
    HedgeHog.hitbox.body.y     = 500;
    HedgeHog.hitbox.body.h     = 30;
    HedgeHog.hitbox.body.w     = 70;
    HedgeHog.hitbox.borderDraw = DBG_FLAG;//for debug
	HedgeHog.hitbox.color      = 0xff00db;//for debug
}

///////////////////////////////////////////////////////////////////////////////
void InitApples()
{
    int i;

    for(i = 0; i < APPLES_NUM; i++)
    {
        Apples[i].radius    = 10 + rand()%9;
        Apples[i].color     = Colors[rand()%(sizeof(Colors)/sizeof(Colors[0]))];
        Apples[i].x0        = 120 + rand()%(WIDTH  - Apples[i].radius - 225);
        Apples[i].y0        = 35 + rand()%(HEIGHT - Apples[i].radius - 380);
        Apples[i].inFalling = FALSE;
        Apples[i].fallSpeed = 5;

        Apples[i].stick.x1    = (Apples[i].x0 * 2 +  Apples[i].radius / 1.25) / 2;
        Apples[i].stick.y1    = Apples[i].y0 -  Apples[i].radius / 2;
        Apples[i].stick.x2    = (Apples[i].x0 * 2 + (15 - rand()%5)) / 2;
        Apples[i].stick.y2    = Apples[i].y0 - (10 + rand()% Apples[i].radius);
        Apples[i].stick.color = 0x7f6938;

        Apples[i].hitbox.body.x = Apples[i].x0 - Apples[i].radius;
        Apples[i].hitbox.body.y = Apples[i].y0;
        Apples[i].hitbox.body.h = Apples[i].radius;
        Apples[i].hitbox.body.w = (Apples[i].x0 + Apples[i].radius*2.9) - Apples[i].x0;

        //for debug
        Apples[i].hitbox.borderDraw = DBG_FLAG;
		Apples[i].hitbox.color      = 0xff00db;
    }
}

///////////////////////////////////////////////////////////////////////////////
void DrawBackground(SDL_Surface *surf)
{
    SDL_Surface *background = NULL;

    background = SDL_LoadBMP(BG_IMAGE);
    if(!background)
    {
        fprintf(stderr, "SDL background failed: %s\n", SDL_GetError());
        return;
    }

    SDL_BlitSurface(background, NULL, surf, NULL);
    SDL_FreeSurface(background);
}

///////////////////////////////////////////////////////////////////////////////
void DrawHedgehog(SDL_Surface *surf)
{
    SDL_Surface *hh = NULL;
    static int cnt = 0, xPos = 0;

    if(!TheEnd)
        sprintf(HedgeHog.image, "Hedgehog_%c%d.bmp", (HedgeHog.direct == RIGHT)?'R':'L', ((cnt++/6)%2)+1);
    hh = SDL_LoadBMP(HedgeHog.image);
    if(!hh)
    {
        fprintf(stderr, "SDL hedgehog failed: %s\n", SDL_GetError());
        return;
    }

    SDL_SetColorKey(hh, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(hh->format, 0, 255, 255));

    if(xPos + hh->w >= 725) HedgeHog.direct = LEFT;
    if(xPos <= 80)          HedgeHog.direct = RIGHT;
    if(TheEnd) HedgeHog.speed = 0;
    
	xPos += HedgeHog.direct * HedgeHog.speed;
	HedgeHog.rect.x = xPos;

    if(HedgeHog.direct == RIGHT) HedgeHog.hitbox.body.x = HedgeHog.rect.x + 20;
    if(HedgeHog.direct == LEFT)  HedgeHog.hitbox.body.x = HedgeHog.rect.x + 40;
    HedgeHog.rect.w = hh->w;
    HedgeHog.rect.h = hh->h;

    SDL_BlitSurface(hh, NULL, surf, &HedgeHog.rect);
    SDL_FreeSurface(hh);

    //for debug
    if(HedgeHog.hitbox.borderDraw)
        Draw_Rect(surf, HedgeHog.hitbox.body.x,
                        HedgeHog.hitbox.body.y,
                        HedgeHog.hitbox.body.w,
                        HedgeHog.hitbox.body.h,
                        HedgeHog.hitbox.color);
}

///////////////////////////////////////////////////////////////////////////////
void DrawApple(SDL_Surface *surf, int num)
{
	if(StartFall) StartingFall();
    if(Apples[num].inFalling) ChangeAppleState(num);

    Draw_Line(surf, Apples[num].stick.x1, Apples[num].stick.y1, Apples[num].stick.x2, Apples[num].stick.y2, Apples[num].stick.color);
    Draw_FillCircle(surf, Apples[num].x0, Apples[num].y0, Apples[num].radius, Apples[num].color);
    Draw_FillCircle(surf, Apples[num].x0 + Apples[num].radius/1.25, Apples[num].y0, Apples[num].radius, Apples[num].color);

    //for debug
    if(Apples[num].hitbox.borderDraw)
        Draw_Rect(surf, Apples[num].hitbox.body.x,
                        Apples[num].hitbox.body.y,
                        Apples[num].hitbox.body.w,
                        Apples[num].hitbox.body.h,
                        Apples[num].hitbox.color);
}

///////////////////////////////////////////////////////////////////////////////
void DrawFrame(SDL_Surface *surf)
{
    int i;

    DrawBackground(surf);
    DrawHedgehog(surf);
    for(i = 0; i < APPLES_NUM; i++)
		DrawApple(surf, i);

    SDL_Flip(surf);
}

///////////////////////////////////////////////////////////////////////////////
void ChangeAppleState(int num)
{
    if(Apples[num].y0 < (HEIGHT-Apples[num].fallSpeed) - Apples[num].radius)
    {
        Apples[num].y0 += Apples[num].fallSpeed;
        Apples[num].stick.y1 += Apples[num].fallSpeed;
        Apples[num].stick.y2 += Apples[num].fallSpeed;
        Apples[num].hitbox.body.y += Apples[num].fallSpeed;
    }
}

///////////////////////////////////////////////////////////////////////////////
BOOL CheckCollision(HitBox apple, HitBox hedgehog)
{	
	if((apple.body.y + apple.body.h) >= hedgehog.body.y &&
        apple.body.y <= hedgehog.body.y + hedgehog.body.h)
        if((apple.body.x + apple.body.w) >= hedgehog.body.x &&
            apple.body.x <= (hedgehog.body.x + hedgehog.body.w))
            return TRUE;

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL FindCollision()
{
	static int tmp = 0;
	tmp++;
    int i;

    for(i = 0; i < APPLES_NUM; i++)
    {
        if(Apples[i].inFalling)
            if(CheckCollision(Apples[i].hitbox, HedgeHog.hitbox))
            {
				Apples[i].inFalling = FALSE;
                return TRUE;
            }
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL RandTimeInterval()
{
	static time_t delay = 0;
	static time_t prev = time(NULL);
	time_t curr = time(NULL);

	if((curr - prev) >= delay){
		//printf("timestamp: %s", ctime((const time_t*)&curr));
		prev = curr;
		delay = rand() % MAX_TIME_INTERVAL;
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL StartingFall()
{
	static int i = 0;

	if(RandTimeInterval() && i < APPLES_NUM){
		Apples[i].inFalling = TRUE;
		i++;
		return TRUE;
	}

	return FALSE;
}