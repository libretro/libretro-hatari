/*
  Hatari - hostscreen.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Host video routines. This file originally came from the Aranym project but
  has been thoroughly reworked for Hatari. However, integration with the rest
  of the Hatari source code is still bad and needs a lot of improvement...
*/
const char HostScreen_fileid[] = "Hatari hostscreen.c : " __DATE__ " " __TIME__;

#include <SDL.h>
#include "main.h"
#include "configuration.h"
#include "control.h"
#include "sysdeps.h"
#include "stMemory.h"
#include "ioMem.h"
#include "hostscreen.h"
#include "resolution.h"
#include "screen.h"
#include "statusbar.h"

#define VIDEL_DEBUG 0

#if VIDEL_DEBUG
#define Dprintf(a) printf a
#else
#define Dprintf(a)
#endif


#define RGB_BLACK     0x00000000
#define RGB_BLUE      0x000000ff
#define RGB_GREEN     0x00ff0000
#define RGB_CYAN      0x00ff00ff
#define RGB_RED       0xff000000
#define RGB_MAGENTA   0xff0000ff
#define RGB_LTGRAY    0xbbbb00bb
#define RGB_GRAY      0x88880088
#define RGB_LTBLUE    0x000000aa
#define RGB_LTGREEN   0x00aa0000
#define RGB_LTCYAN    0x00aa00aa
#define RGB_LTRED     0xaa000000
#define RGB_LTMAGENTA 0xaa0000aa
#define RGB_YELLOW    0xffff0000
#define RGB_LTYELLOW  0xaaaa0000
#define RGB_WHITE     0xffff00ff

//DEB RETRO HACK
#ifdef RETRO
	extern SDL_Surface sdlscrn;
	#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))
#endif
//FIN RETRO HACK

/* TODO: put these hostscreen globals to some struct */
static Uint32 sdl_videoparams;
static int hs_width, hs_height, hs_width_req, hs_height_req, hs_bpp;
static bool   doUpdate; // the HW surface is available -> the SDL need not to update the surface after ->pixel access

static void HostScreen_remapPalette(void);

static struct { // TOS palette (bpp < 16) to SDL color mapping
	//DEB RETRO HACK
	#ifndef RETRO
	SDL_Color	standard[256];
	#else
	Uint32	standard[256];
	#endif
	//FIN RETRO HACK

	Uint32		native[256];
} palette;


static const Uint32 default_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};


void HostScreen_Init(void)
{
	int i;
	for(i = 0; i < 256; i++) {
		Uint32 color = default_palette[i%16];
//DEB RETRO HACK
	#ifndef RETRO
		palette.standard[i].r = color >> 24;
		palette.standard[i].g = (color >> 16) & 0xff;
		palette.standard[i].b = color & 0xff;;
	#else
		palette.standard[i] = color ;
	#endif
//FIN RETRO HACK
	}
}

void HostScreen_UnInit(void)
{
}


void HostScreen_toggleFullScreen(void)
{
//DEB RETRO HACK
#ifndef RETRO
	sdl_videoparams ^= SDL_FULLSCREEN;
	Dprintf(("Fullscreen = %s, width = %d, height = %d, bpp = %d\n",
		 sdl_videoparams&SDL_FULLSCREEN?"true":"false", hs_width_req, hs_height_req, hs_bpp));

	HostScreen_setWindowSize(hs_width_req, hs_height_req, hs_bpp);
	/* force screen redraw */
	HostScreen_update1(true);
#endif
//FIN RETRO HACK
}


void HostScreen_setWindowSize(int width, int height, int bpp)
{
	int screenwidth, screenheight, maxw, maxh;
	int scalex, scaley, sbarheight;

	if (bpp == 24)
		bpp = 32;
//DEB RETRO HACK
#ifndef RETRO

	/* constrain size request to user's desktop size */
	Resolution_GetDesktopSize(&maxw, &maxh);
	scalex = scaley = 1;
	while (width > maxw*scalex) {
		scalex *= 2;
	}
	while (height > maxh*scalex) {
		scalex *= 2;
	}
	if (scalex * scaley > 1) {
		fprintf(stderr, "WARNING: too large screen size %dx%d -> divided by %dx%d!\n",
			width, height, scalex, scaley);
		width /= scalex;
		height /= scaley;
	}

	Resolution_GetLimits(&maxw, &maxh, &bpp, ConfigureParams.Screen.bKeepResolution);
	nScreenZoomX = nScreenZoomY = 1;
	
	if (ConfigureParams.Screen.bAspectCorrect) {
		/* Falcon (and TT) pixel scaling factors seem to 2^x
		 * (quarter/half pixel, interlace/double line), so
		 * do aspect correction as 2's exponent.
		 */
		while (nScreenZoomX*width < height &&
		       2*nScreenZoomX*width < maxw) {
			nScreenZoomX *= 2;
		}
		while (2*nScreenZoomY*height < width &&
		       2*nScreenZoomY*height < maxh) {
			nScreenZoomY *= 2;
		}
		if (nScreenZoomX*nScreenZoomY > 2) {
			fprintf(stderr, "WARNING: strange screen size %dx%d -> aspect corrected by %dx%d!\n",
				width, height, nScreenZoomX, nScreenZoomY);
		}
	}

	/* then select scale as close to target size as possible
	 * without having larger size than it
	 */
	scalex = maxw/(nScreenZoomX*width);
	scaley = maxh/(nScreenZoomY*height);
	if (scalex > 1 && scaley > 1) {
		/* keep aspect ratio */
		if (scalex < scaley) {
			nScreenZoomX *= scalex;
			nScreenZoomY *= scalex;
		} else {
			nScreenZoomX *= scaley;
			nScreenZoomY *= scaley;
		}
	}

	hs_width_req = width;
	hs_height_req = height;
	width *= nScreenZoomX;
	height *= nScreenZoomY;

	/* get statusbar size for this screen size */
	sbarheight = Statusbar_GetHeightForSize(width, height);
	screenheight = height + sbarheight;
	screenwidth = width;

	/* get resolution corresponding to these */
	Resolution_Search(&screenwidth, &screenheight, &bpp);
	/* re-calculate statusbar height for this resolution */
	sbarheight = Statusbar_SetHeight(screenwidth, screenheight-sbarheight);

	hs_bpp = bpp;
	/* videl.c might scale things differently in fullscreen than
	 * in windowed mode because this uses screensize instead of using
	 * the aspect scaled sizes directly, but it works better this way.
	 */
	hs_width = screenwidth;
	hs_height = screenheight - sbarheight;

	if (sdlscrn && (!bpp || sdlscrn->format->BitsPerPixel == bpp) &&
	    sdlscrn->w == (signed)screenwidth && sdlscrn->h == (signed)screenheight &&
	    (sdlscrn->flags&SDL_FULLSCREEN) == (sdl_videoparams&SDL_FULLSCREEN))
	{
		/* same host screen size despite Atari resolution change,
		 * -> no time consuming host video mode change needed
		 */
		if (screenwidth > width || screenheight > height+sbarheight) {
			/* Atari screen smaller than host -> clear screen */
			SDL_Rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.w = sdlscrn->w;
			rect.h = sdlscrn->h - sbarheight;
			SDL_FillRect(sdlscrn, &rect, SDL_MapRGB(sdlscrn->format, 0, 0, 0));
			/* re-calculate variables in case height + statusbar height
			 * don't anymore match SDL surface size (there's an assert
			 * for that)
			 */
			Statusbar_Init(sdlscrn);
		}
		// check in case switched from VDI to Hostscreen
		doUpdate = ( sdlscrn->flags & SDL_HWSURFACE ) == 0;
		return;
	}

	if (bInFullScreen) {
		/* un-embed the Hatari WM window for fullscreen */
		Control_ReparentWindow(screenwidth, screenheight, bInFullScreen);

		sdl_videoparams = SDL_SWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN;
	} else {
		sdl_videoparams = SDL_SWSURFACE|SDL_HWPALETTE;
	}
#ifdef _MUDFLAP
	if (sdlscrn) {
		__mf_unregister(sdlscrn->pixels, sdlscrn->pitch*sdlscrn->h, __MF_TYPE_GUESS);
	}
#endif
	sdlscrn = SDL_SetVideoMode(screenwidth, screenheight, bpp, sdl_videoparams);
#ifdef _MUDFLAP
	__mf_register(sdlscrn->pixels, sdlscrn->pitch*sdlscrn->h, __MF_TYPE_GUESS, "SDL pixels");
#endif
	if (!bInFullScreen) {
		/* re-embed the new Hatari SDL window */
		Control_ReparentWindow(screenwidth, screenheight, bInFullScreen);
	}

	// In case surface format changed, update SDL palette & remap the native palette
	HostScreen_updatePalette(256);
	HostScreen_remapPalette();

	// redraw statusbar
	Statusbar_Init(sdlscrn);

	Dprintf(("Surface Pitch = %d, width = %d, height = %d\n", sdlscrn->pitch, sdlscrn->w, sdlscrn->h));
	Dprintf(("Must Lock? %s\n", SDL_MUSTLOCK(sdlscrn) ? "YES" : "NO"));

	// is the SDL_update needed?
	doUpdate = ( sdlscrn->flags & SDL_HWSURFACE ) == 0;

	Dprintf(("Pixel format:bitspp=%d, tmasks r=%04x g=%04x b=%04x"
			", tshifts r=%d g=%d b=%d"
			", tlosses r=%d g=%d b=%d\n",
			sdlscrn->format->BitsPerPixel,
			sdlscrn->format->Rmask, sdlscrn->format->Gmask, sdlscrn->format->Bmask,
			sdlscrn->format->Rshift, sdlscrn->format->Gshift, sdlscrn->format->Bshift,
			sdlscrn->format->Rloss, sdlscrn->format->Gloss, sdlscrn->format->Bloss));

	Main_WarpMouse(sdlscrn->w/2,sdlscrn->h/2);
#else
	nScreenZoomX = nScreenZoomY = 1;

	width = retrow;
	screenheight = retroh;
	bpp=16;
	hs_width = retrow;
	hs_height = retroh;
	hs_bpp = 16;

	// update the surface's palette
	HostScreen_updatePalette( 256 );

	doUpdate = 1;// 0;

	Main_WarpMouse(sdlscrn.w/2,sdlscrn.h/2);


#endif
//FIN RETRO HACK
}


void HostScreen_update1(bool forced)
{
	if ( !forced && !doUpdate ) // the HW surface is available
		return;

//DEB RETRO HACK
#ifndef RETRO
	SDL_UpdateRect( sdlscrn, 0, 0, hs_width, hs_height );
#endif
//FIN RETRO HACK
}


Uint32 HostScreen_getBpp(void)
{
//DEB RETRO HACK
#ifndef RETRO
	return sdlscrn->format->BytesPerPixel;
#else
	return 2;
#endif
//FIN RETRO HACK
}

Uint32 HostScreen_getPitch(void)
{
//DEB RETRO HACK
#ifndef RETRO
	return sdlscrn->pitch;
#else
	return 2*retrow;
#endif
//FIN RETRO HACK

}

Uint32 HostScreen_getWidth(void)
{
//DEB RETRO HACK
#ifndef RETRO
	return hs_width;
#else
	return retrow;
#endif
//FIN RETRO HACK
}

Uint32 HostScreen_getHeight(void)
{
//DEB RETRO HACK
#ifndef RETRO
	return hs_height;
#else
	return retroh;
#endif
//FIN RETRO HACK
}

Uint8 *HostScreen_getVideoramAddress(void)
{
//DEB RETRO HACK
#ifndef RETRO
	return sdlscrn->pixels;
#else
	return sdlscrn.bitmap;
#endif
//FIN RETRO HACK
}

//DEB RETRO HACK
#ifndef RETRO
SDL_PixelFormat *HostScreen_getFormat(void)
{
#else
int HostScreen_getFormat(void)
{
#endif

#ifndef RETRO
	return sdlscrn->format;
#else
	return 16;
#endif
//FIN RETRO HACK
}

void HostScreen_setPaletteColor(Uint8 idx, Uint8 red, Uint8 green, Uint8 blue)
{
//DEB RETRO HACK
#ifndef RETRO
	// set the SDL standard RGB palette settings
	palette.standard[idx].r = red;
	palette.standard[idx].g = green;
	palette.standard[idx].b = blue;
	// convert the color to native
	palette.native[idx] = SDL_MapRGB( sdlscrn->format, red, green, blue );
#else
	Uint16 ret= RGB565(red>>3, green>>3, blue>>3) ;
    	palette.standard[idx]= ret;
	// convert the color to native
	palette.native[idx] = ret;

#endif
//FIN RETRO HACK
}

//DEB RETRO HACK
#ifdef RETRO
Uint32 HostScreen_getColor(Uint32 red, Uint32 green, Uint32 blue)
{
     	Uint16 ret= RGB565(red>>3, green>>3, blue>>3) ;

	return  ret;
}
#endif
//FIN RETRO HACK

Uint32 HostScreen_getPaletteColor(Uint8 idx)
{
	return palette.native[idx];
}

void HostScreen_updatePalette(int colorCount)
{
//DEB RETRO HACK
#ifndef RETRO
	SDL_SetColors( sdlscrn, palette.standard, 0, colorCount );
#endif
//FIN RETRO HACK
}

static void HostScreen_remapPalette(void)
{
//DEB RETRO HACK
#ifndef RETRO
	int i;
	Uint32 *native = palette.native;
	SDL_Color *standard = palette.standard;
	SDL_PixelFormat *fmt = sdlscrn->format;

	for(i = 0; i < 256; i++, native++, standard++) {
		*native = SDL_MapRGB(fmt, standard->r, standard->g, standard->b);
	}
#endif
//FIN RETRO HACK
}

bool HostScreen_renderBegin(void)
{
//DEB RETRO HACK
#ifndef RETRO
	if (SDL_MUSTLOCK(sdlscrn))
		if (SDL_LockSurface(sdlscrn) < 0) {
			printf("Couldn't lock surface to refresh!\n");
			return false;
		}
#endif
//FIN RETRO HACK
	return true;
}

void HostScreen_renderEnd(void)
{
//DEB RETRO HACK
#ifndef RETRO
	if (SDL_MUSTLOCK(sdlscrn))
		SDL_UnlockSurface(sdlscrn);
	Statusbar_Update(sdlscrn);
#endif
//FIN RETRO HACK
}
