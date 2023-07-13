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

#ifndef _DRAWTEXT_H

#define _DRAWTEXT_H

#include <X11/Xlib.h>
#include "typedef.h"

extern CharEntry charfield[40][32];
extern GC gc_text;
extern XGCValues gc_text_values;
extern unsigned long gc_text_mask;

extern void ClearCharField(int bg, Bool todraw);
extern void DrawTextField();
extern void WriteTextStr(const unsigned char *str, int x, int y, 
			 int fg, int bg);
extern void Mark_CharField(XExposeEvent *xexpose, int max_y);

#endif
