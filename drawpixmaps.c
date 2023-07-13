/* XDigger  Copyright (C) 1988-99 Alexander Lang.

XDigger is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

XDigger is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "xdigger.h"
#include "drawpixmaps.h"
#include "pixmaps.h"

Pixmap /*main_win_icon_pixmap,*/ pixmaps[CountPixmaps],
  iconpixmaps[CountIconPixmaps], xbm_charset[180],
  /*xpm_charset,*/ xpm_cur_digger_s, xpm_cur_digger_m;

Cursor cursor_digger, cursor_watch;

Visual *visual;
XImage *image;
int xdefaultdepth;
Bool AllocColorFailed = False;
/* Colormap colormap; */

Pixmap MakePixmap(char **pixmap_xpm, int mag)
{
  Pixmap pixmap;
  /* Colormap colormap; */
  XColor color; /*, exact;*/
  int xpm_width, xpm_height, xpm_num_colors, xpm_chars_per_pixel;
  int i, j, x, y, z, f, countpixel;
  int char2color[256];
  char c, *value;
/*   float red, green, blue; */

  sscanf(pixmap_xpm[0], "%d %d %d %d", &xpm_width, &xpm_height,
	 &xpm_num_colors, &xpm_chars_per_pixel);

  for (z=1; z<=xpm_num_colors; z++)
    {
      value = strchr(pixmap_xpm[z], '#');
      XParseColor(display, colormap, value, &color);
/*       red = (float)color.red / 0x10000; */
/*       color.pixel = xstdclrmap.base_pixel + */
/* 	((unsigned long)(0.5+(red*xstdclrmap.red_max))*xstdclrmap.red_mult); */
/* 	(color.red * xstdclrmap.red_max / 0x10000) * xstdclrmap.red_mult + */
/* 	(color.green * xstdclrmap.green_max / 0x10000) * xstdclrmap.green_mult+ */
/* 	(color.blue * xstdclrmap.blue_max / 0x10000) * xstdclrmap.blue_mult; */
      if (!XAllocColor(display, colormap, &color))
	AllocColorFailed = True;
      char2color[(unsigned char)pixmap_xpm[z][0]] = color.pixel;
    }

  countpixel = 0;

  for (y=0; y<xpm_height; y++) for (x=0; x<xpm_width; x++)
    {
      c = pixmap_xpm[y+xpm_num_colors+1][x];
      f = char2color[(unsigned char)c];
      for (j=0; j<mag; j++) for (i=0; i<mag; i++)
	{
	  XPutPixel(image, x*mag+i, y*mag+j, f);
	}
    }
/*   pixmap = XCreatePixmap(display, mainwindow, 16*mag, 16*mag, xdefaultdepth); */
  pixmap = XCreatePixmap(display, mainwindow, xpm_width*mag, xpm_height*mag, xdefaultdepth);
  { /* Kontrolle ob pixmap gueltig: */
    Window r;
    int x, y, w, h, b, d;
    if (XGetGeometry(display, mainwindow, &r, &x, &y, &w, &h, &b, &d) == 
	BadAlloc)
      {
	fprintf(stderr, "%s: Unvorhergesehener Fehler beim Erzeugen der "
                "Pixmaps!\n", progname);
	exit(42);
      }
  }
/*   XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, 16*mag, 16*mag); */
  XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, xpm_width*mag, xpm_height*mag);
  return(pixmap);
} /* Pixmap MakePixmap(char **pixmap_xpm, int mag) */

Pixmap XCreateMagBitmapFromData(data, width, height, mag)
char *data;
unsigned int width, height, mag;
{
  /*char *imdata;*/
  XImage *image;
  Pixmap pixmap;
  GC gc;
  XGCValues gcvalues;
  unsigned long gcvaluemask;
  unsigned char *src, b;
  unsigned long f;
  int i, j, k, x, y, dx, dy;
  Bool p;

  /*
  imdata = malloc(width*height*mag*mag/8);
  image = XCreateImage(display, visual, 1, XYBitmap, 0, imdata,
		       width*mag, height*mag, 8, 0);
  */
  image = XCreateImage(display, visual, 1, XYBitmap, 0, NULL,
		       width*mag, height*mag, 8, 0);
  if (image != NULL)
    image->data = malloc(image->bytes_per_line * image->height);

  src = data;
  for (y=0; y<height; y++)
    for (x=0; x<(width+1)/8; x++)
      {
	b = *src++;
	for (k=0; k<8; k++)
	  {
	    p = b & 1;
	    b = b >> 1;
	    dx = x*8 + k;
	    dy = y;
	    if (p) f = 1/*fg*/; else f = 0/*bg*/;
	    for (j=0; j<mag; j++) for (i=0; i<mag; i++)
	      XPutPixel(image, dx*mag+i, dy*mag+j, f);
	  }
      }

  pixmap = 
    XCreatePixmap(display, mainwindow, width*mag, height*mag, 1);
  gcvalues.foreground = 1;
  gcvalues.background = 0;
  gcvaluemask = GCForeground | GCBackground;
  gc = XCreateGC(display, pixmap, gcvaluemask, &gcvalues);
  XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, width*mag, height*mag);
  XDestroyImage(image);
  XFreeGC(display, gc);

  return(pixmap);
} /* Pixmap XCreateMagBitmapFromData(data, width, height, mag) */

void CreateImage()
{
  /*char *data;*/
  int maxsize;
  
  /* Image erzeugen */
  maxsize = 32;
  if (maxsize < 16*xpmmag) maxsize = 16*xpmmag;
  if (maxsize < 16*iconmag) maxsize = 16*iconmag;
  /*
  data = malloc(maxsize*maxsize*xdefaultdepth/8);
  image = XCreateImage(display, visual, xdefaultdepth, ZPixmap, 0, data,
		       maxsize, maxsize, 8, 0);
  */
  image = XCreateImage(display, visual, xdefaultdepth, ZPixmap, 0, NULL,
		       maxsize, maxsize, 8, 0);
  if (image != NULL)
    image->data = malloc(image->bytes_per_line * image->height);
  else {
    fprintf(stderr, "%s: Unvorhergesehener Fehler beim Erzeugen des Images!\n",
            progname);
    exit(42);
  }
}

void Make16x16Pixmaps()
{
  pixmaps[PXID_NOTHING]          = MakePixmap(nothing_xpm, xpmmag);
  pixmaps[PXID_STONE]            = MakePixmap(stone_xpm, xpmmag);
  pixmaps[PXID_GROUND]           = MakePixmap(ground_xpm, xpmmag);
  pixmaps[PXID_GHOST]            = MakePixmap(ghost_xpm, xpmmag);
  pixmaps[PXID_GHOST_LEFT]       = MakePixmap(ghost_left_xpm, xpmmag);
  pixmaps[PXID_GHOST_RIGHT]      = MakePixmap(ghost_right_xpm, xpmmag);
  pixmaps[PXID_GHOST_UP]         = MakePixmap(ghost_up_xpm, xpmmag);
  pixmaps[PXID_DIAMOND]          = MakePixmap(diamond_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK0]   = MakePixmap(diamond0_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK1]   = MakePixmap(diamond1_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK2]   = MakePixmap(diamond2_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK3]   = MakePixmap(diamond3_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK4]   = MakePixmap(diamond4_xpm, xpmmag);
  pixmaps[PXID_DIAMOND_BLINK5]   = MakePixmap(diamond5_xpm, xpmmag);
  pixmaps[PXID_WALL]             = MakePixmap(wall_xpm, xpmmag);
  pixmaps[PXID_DIGGER]           = MakePixmap(digger_xpm, xpmmag);
  pixmaps[PXID_DIGGER_LEFT0]     = MakePixmap(digger_left0_xpm, xpmmag);
  pixmaps[PXID_DIGGER_LEFT1]     = MakePixmap(digger_left1_xpm, xpmmag);
  pixmaps[PXID_DIGGER_LEFT2]     = MakePixmap(digger_left2_xpm, xpmmag);
  pixmaps[PXID_DIGGER_LEFT3]     = MakePixmap(digger_left3_xpm, xpmmag);
  pixmaps[PXID_DIGGER_RIGHT0]    = MakePixmap(digger_right0_xpm, xpmmag);
  pixmaps[PXID_DIGGER_RIGHT1]    = MakePixmap(digger_right1_xpm, xpmmag);
  pixmaps[PXID_DIGGER_RIGHT2]    = MakePixmap(digger_right2_xpm, xpmmag);
  pixmaps[PXID_DIGGER_RIGHT3]    = MakePixmap(digger_right3_xpm, xpmmag);
  pixmaps[PXID_DIGGER_UP1]       = MakePixmap(digger_up1_xpm, xpmmag);
  pixmaps[PXID_DIGGER_UP2]       = MakePixmap(digger_up2_xpm, xpmmag);
  pixmaps[PXID_DIGGER_DOWN1]     = MakePixmap(digger_down1_xpm, xpmmag);
  pixmaps[PXID_DIGGER_DOWN2]     = MakePixmap(digger_down2_xpm, xpmmag);
  pixmaps[PXID_DIGGER_FOOT]      = MakePixmap(digger_foot_xpm, xpmmag);
  pixmaps[PXID_DIGGER_EYE1]      = MakePixmap(digger_eye1_xpm, xpmmag);
  pixmaps[PXID_DIGGER_EYE2]      = MakePixmap(digger_eye2_xpm, xpmmag);
  pixmaps[PXID_DIGGER_DEATH]     = MakePixmap(digger_death_xpm, xpmmag);
  pixmaps[PXID_EXIT]             = MakePixmap(exit_xpm, xpmmag);
  pixmaps[PXID_CHANGER]          = MakePixmap(changer_xpm, xpmmag);
}

void Make16x16Pixmaps_Editor()
{
  CreateImage();
  pixmaps[PXID_GHOST_DOWN_180]   = MakePixmap(ghost_down_180_xpm, xpmmag);
  pixmaps[PXID_GHOST_UP_180]     = MakePixmap(ghost_up_180_xpm, xpmmag);
  pixmaps[PXID_GHOST_LEFT_180]   = MakePixmap(ghost_left_180_xpm, xpmmag);
  pixmaps[PXID_GHOST_RIGHT_180]  = MakePixmap(ghost_right_180_xpm, xpmmag);
  pixmaps[PXID_GHOST_DOWN_L]     = MakePixmap(ghost_down_l_xpm, xpmmag);
  pixmaps[PXID_GHOST_UP_L]       = MakePixmap(ghost_up_l_xpm, xpmmag);
  pixmaps[PXID_GHOST_LEFT_L]     = MakePixmap(ghost_left_l_xpm, xpmmag);
  pixmaps[PXID_GHOST_RIGHT_L]    = MakePixmap(ghost_right_l_xpm, xpmmag);
  pixmaps[PXID_GHOST_DOWN_R]     = MakePixmap(ghost_down_r_xpm, xpmmag);
  pixmaps[PXID_GHOST_UP_R]       = MakePixmap(ghost_up_r_xpm, xpmmag);
  pixmaps[PXID_GHOST_LEFT_R]     = MakePixmap(ghost_left_r_xpm, xpmmag);
  pixmaps[PXID_GHOST_RIGHT_R]    = MakePixmap(ghost_right_r_xpm, xpmmag);
  pixmaps[PXID_GHOST_DOWN_LR]    = MakePixmap(ghost_down_lr_xpm, xpmmag);
  pixmaps[PXID_GHOST_UP_LR]      = MakePixmap(ghost_up_lr_xpm, xpmmag);
  pixmaps[PXID_GHOST_LEFT_LR]    = MakePixmap(ghost_left_lr_xpm, xpmmag);
  pixmaps[PXID_GHOST_RIGHT_LR]   = MakePixmap(ghost_right_lr_xpm, xpmmag);
  XDestroyImage(image);
}

void Make16x16Pixmaps_Icon()
{
  iconpixmaps[PXID_ICON_NOTHING]     = MakePixmap(nothing_xpm, iconmag);
  iconpixmaps[PXID_ICON_DIGGER]      = MakePixmap(digger_xpm, iconmag);
  iconpixmaps[PXID_ICON_DIGGER_FOOT] = MakePixmap(digger_foot_xpm, iconmag);
  iconpixmaps[PXID_ICON_DIGGER_EYE1] = MakePixmap(digger_eye1_xpm, iconmag);
  iconpixmaps[PXID_ICON_DIGGER_EYE2] = MakePixmap(digger_eye2_xpm, iconmag);
}

void MakeXPixmaps(Bool first_time)
{
  /*XColor fg_color, bg_color;*/
  int i;

  if (first_time)
  {
    visual = XDefaultVisual(display, screen);
    xdefaultdepth = XDefaultDepth(display, screen);
    colormap = DefaultColormap(display, screen);
  }

  /* Digger-Cursor */
/*  xpm_cur_digger_s = 
    XCreatePixmapFromBitmapData(display, mainwindow, cur_digger_s_bits,
				cur_digger_s_width, cur_digger_s_height,
				BlackPixel(display, screen),
				WhitePixel(display, screen), 1);

  xpm_cur_digger_m =
    XCreatePixmapFromBitmapData(display, mainwindow, cur_digger_m_bits,
				cur_digger_m_width, cur_digger_m_height,
				BlackPixel(display, screen),
				WhitePixel(display, screen), 1);

  XParseColor(display, colormap, "Black", &fg_color);
  XParseColor(display, colormap, "#c0c0c0", &bg_color);
  cursor_digger = 
    XCreatePixmapCursor(display, xpm_cur_digger_s, xpm_cur_digger_m, 
			&fg_color, &bg_color, 7, 7);
  cursor_watch =
    XCreateFontCursor(display, XC_watch);

  XDefineCursor(display, mainwindow, cursor_digger); */

  /* Charset */
  for (i=0; i<180; i++)
  {
    xbm_charset[i] = 
      XCreateMagBitmapFromData(charset_bits+i*8, 8, 8, xpmmag);
  }

  /* pixmaps erzeugen */
  CreateImage();
  bzero(pixmaps, sizeof(pixmaps));
  Make16x16Pixmaps();
  if (first_time)
  {
    Make16x16Pixmaps_Icon();
    /*main_win_icon_pixmap = MakePixmap(xdigger_xpm, 1);*/
  }
  XDestroyImage(image);
  
  if (first_time && AllocColorFailed)
    fprintf(stderr, "%s: can't alloc some colors.\n", progname);
} /* MakeXPixmaps() */

void FreeXPixmaps()
{
  int i;
  
  for (i=0; i<180; i++)
    XFreePixmap(display, xbm_charset[i]);
  for (i=0; i<CountPixmaps; i++)
    if (pixmaps[i] != 0) XFreePixmap(display, pixmaps[i]);
  bzero(pixmaps, sizeof(pixmaps));
}

void CopyOnePixmap_Window(Window window, unsigned char pixmap_id, int x, int y)
{
  XCopyArea(display, pixmaps[pixmap_id], window, gc, 0, 0,
	    16*xpmmag, 16*xpmmag, x*16*xpmmag, y*16*xpmmag);
} /* CopyOnePixmap_Window(Window window, unsigned char pixmap_id,
     int x, int y) */

void DrawOnePixmap(unsigned char pixmap_id, int x, int y)
{
  XCopyArea(display, pixmaps[pixmap_id], mainwindow, gc, 0, 0,
	    16*xpmmag, 16*xpmmag, x*16*xpmmag, (y+vertvar+1)*16*xpmmag);
}

void ResizeXPixmaps(int newmag)
{
  if (newmag == xpmmag) return;

  /*fprintf(stderr, "Resize %d -> %d \n", xpmmag, newmag);
  fflush(stderr);*/

  xpmmag = newmag;
  FreeXPixmaps();
  MakeXPixmaps(False);
  if (toolswindow != 0)
  {
    Make16x16Pixmaps_Editor();
    XResizeWindow(display, toolswindow, 4*16*xpmmag, 6*16*xpmmag);
  }
}
