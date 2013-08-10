/*
  Hatari - main.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_MAIN_H
#define HATARI_MAIN_H

//RETO HACK
#ifdef RETRO
#include <ctype.h>

#ifdef PS3PORT
#include <sdk_version.h>
#include <cell.h>
#include <stdio.h>
#include <string.h>
#define	getcwd(a,b)	"/"
#include <unistd.h> //stat() is defined here
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#define F_OK 0
#define ftello ftell
#define chdir(a) 0
#define getenv(a)	"/dev_hdd0/HOMEBREW/ST/"
#define tmpfile() NULL
#endif
//#define PROG_NAME "Hatari devel (" __DATE__ ")"
#endif
//RETRO HACK

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <SDL_types.h>
#include <stdbool.h>

#if __GNUC__ >= 3
# define likely(x)      __builtin_expect (!!(x), 1)
# define unlikely(x)    __builtin_expect (!!(x), 0)
#else
# define likely(x)      (x)
# define unlikely(x)    (x)
#endif

#ifdef WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

#define CALL_VAR(func)  { ((void(*)(void))func)(); }

#define ARRAYSIZE(x) (int)(sizeof(x)/sizeof(x[0]))

/* 68000 operand sizes */
#define SIZE_BYTE  1
#define SIZE_WORD  2
#define SIZE_LONG  4

/* The 8 MHz CPU frequency */
#define CPU_FREQ   8012800

extern bool bQuitProgram;

extern bool Main_PauseEmulation(bool visualize);
extern bool Main_UnPauseEmulation(void);
extern void Main_RequestQuit(void);
extern void Main_SetRunVBLs(Uint32 vbls);
extern void Main_WaitOnVbl(void);
extern void Main_WarpMouse(int x, int y);
extern void Main_EventHandler(void);
extern void Main_SetTitle(const char *title);

#endif /* ifndef HATARI_MAIN_H */
