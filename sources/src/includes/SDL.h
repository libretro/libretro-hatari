
#ifndef HATARI_SDL_H
#define HATARI_SDL_H

//RETRO HACK

extern int Reset_Cold(void);
extern int Reset_Warm(void);

#include <time.h>
#include "SDL_types.h"

#define SDL_Delay(a) usleep((a)*1000)

#define SDL_CloseAudio();
#define SDL_LockAudio();
#define SDL_UnlockAudio();
#define SDL_PauseAudio(a);

#ifdef PS3PORT

#if defined(__CELLOS_LV2__) 
#include <unistd.h> //stat() is defined here
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#define F_OK 0
#endif

#include "sys/sys_time.h"
#include "sys/timer.h"
#define usleep  sys_timer_usleep
#endif

#define SDL_GetTicks  GetTicks 
#include <SDL_types.h>

typedef struct{
     Sint16 x, y;
     Uint16 w, h;
} SDL_Rect;

typedef struct
{
    int w, h;
    int stride;    
    unsigned char * bitmap;
} SDL_Surface;

#endif
