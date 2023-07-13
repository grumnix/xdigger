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

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "typedef.h"
#include "xdigger.h"
#include "drawpixmaps.h"

CharEntry charfield[40][32];
GC gc_text;
XGCValues gc_text_values;
unsigned long gc_text_mask;

void ClearCharField(int bg, Bool todraw)
{
  int x, y;
    for (y=0; y<32; y++) for (x=0; x<40; x++)
      {
	charfield[x][y].c = '\0';
	charfield[x][y].bg = bg;
	charfield[x][y].todraw = todraw;
      }
} /* ClearCharField(int bg, Bool todraw) */

void DrawBitmapChar(GC gc, unsigned char c, int x, int y)
{
  if (c<0x20) c=0x20;
/*   XCopyPlane(display, xbm_charset, mainwindow, gc, 0,  */
/* 	     (c-0x20)*8*xpmmag, 8*xpmmag, 8*xpmmag, x*8*xpmmag, y*8*xpmmag, 1); */
  XCopyPlane(display, xbm_charset[c-0x20], mainwindow, gc,
	     0, 0, 8*xpmmag, 8*xpmmag, x*8*xpmmag, y*8*xpmmag, 1);
} /* DrawBitmapChar(GC gc, unsigned char c, int x, int y) */

void DrawOneCharEntry(int x, int y)
{
  if ((gc_text_values.foreground != charfield[x][y].fg) ||
      (gc_text_values.background != charfield[x][y].bg))
    {
      gc_text_values.foreground = charfield[x][y].fg;
      gc_text_values.background = charfield[x][y].bg;
      gc_text_mask = GCForeground | GCBackground;
      XChangeGC(display, gc_text, gc_text_mask, &gc_text_values);
    }
  DrawBitmapChar(gc_text, charfield[x][y].c, x, y);
} /* DrawOneCharEntry(int x, int y) */

void DrawTextField()
{
  int x, y;

  for (y=0; y<32; y++) for (x=0; x<40; x++)
    if (charfield[x][y].todraw)
      {
	DrawOneCharEntry(x, y);
	charfield[x][y].todraw = False;
      }
/*   XFlush(display); */
} /* DrawTextField() */

void WriteTextStr(const unsigned char *str, int x, int y, int fg, int bg)
{
  int i, len;

  len = strlen(str);
  for (i=0; i<len; i++)
    {
      charfield[x+i][y].c      = str[i];
      charfield[x+i][y].fg     = fg;
      charfield[x+i][y].bg     = bg;
      charfield[x+i][y].todraw = True;
    }
} /* WriteTextStr(const unsigned char *str, int x, int y, int fg, int bg) */

void Mark_CharField(XExposeEvent *xexpose, int max_y)
{
  int x, xa, xe, y, ya, ye;

  /*fprintf(stderr, "Mark %d \n", xpmmag);
  fflush(stderr);*/
  xa = (xexpose->x / (8*xpmmag));
  xe = ((xexpose->x + xexpose->width - 1) / (8*xpmmag));
  if (xa > 39) return;
  if (xe > 39) xe = 39;
  ya = (xexpose->y / (8*xpmmag));
  ye = ((xexpose->y + xexpose->height - 1) / (8*xpmmag));
  if (ya > max_y) return;
  if (ye > max_y) ye = max_y;
  for (y=ya; y<=ye; y++) for (x=xa; x<=xe; x++)
    charfield[x][y].todraw = True;
} /* Mark_CharField(XExposeEvent *xexpose, int max_y) */
