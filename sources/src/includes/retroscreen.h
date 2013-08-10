#ifndef RETROSCREEN_H
#define RETROSCREEN_H 1

#ifdef AND
#define RSCRW 640
#define RSCRH 480
#else
//TODO FIX 832*576 full border screen
#define RSCRW 640//832
#define RSCRH 480//576
#endif

#define TEX_WIDTH RSCRW
#define TEX_HEIGHT RSCRH
#define CROP_WIDTH RSCRW
#define CROP_HEIGHT (RSCRH-80)
#define VIRTUAL_WIDTH  RSCRW
#define retrow RSCRW
#define retroh RSCRH

#endif
