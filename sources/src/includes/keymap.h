/*
  Hatari - keymap.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_KEYMAP_H
#define HATARI_KEYMAP_H
//DEB RETRO HACK
#ifndef RETRO
#include <SDL_keyboard.h>
#endif
extern void Keymap_Init(void);
#ifndef RETRO
extern char Keymap_RemapKeyToSTScanCode(SDL_keysym* pKeySym);
#endif
extern void Keymap_LoadRemapFile(char *pszFileName);
extern void Keymap_DebounceAllKeys(void);
#ifndef RETRO
extern void Keymap_KeyDown(SDL_keysym *sdlkey);
extern void Keymap_KeyUp(SDL_keysym *sdlkey);
#endif
//FIN RETRO HACK
extern void Keymap_SimulateCharacter(char asckey, bool press);

#endif
