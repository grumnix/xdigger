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

#ifndef _DRAWPIXMAPS_H

#define _DRAWPIXMAPS_H

#define PXID_NOTHING           0
#define PXID_STONE             1
#define PXID_GROUND            2
#define PXID_GHOST             3
#define PXID_GHOST_LEFT        4
#define PXID_GHOST_RIGHT       5
#define PXID_GHOST_UP          6
#define PXID_DIAMOND           7
#define PXID_DIAMOND_BLINK0    8
#define PXID_DIAMOND_BLINK1    9
#define PXID_DIAMOND_BLINK2   10
#define PXID_DIAMOND_BLINK3   11
#define PXID_DIAMOND_BLINK4   12
#define PXID_DIAMOND_BLINK5   13
#define PXID_WALL             16
#define PXID_DIGGER           17
#define PXID_DIGGER_LEFT0     18
#define PXID_DIGGER_LEFT1     19
#define PXID_DIGGER_LEFT2     20
#define PXID_DIGGER_LEFT3     21
#define PXID_DIGGER_RIGHT0    22
#define PXID_DIGGER_RIGHT1    23
#define PXID_DIGGER_RIGHT2    24
#define PXID_DIGGER_RIGHT3    25
#define PXID_DIGGER_UP1       26
#define PXID_DIGGER_UP2       27
#define PXID_DIGGER_DOWN1     28
#define PXID_DIGGER_DOWN2     29
#define PXID_DIGGER_FOOT      30
#define PXID_DIGGER_EYE1      31
#define PXID_DIGGER_EYE2      32
#define PXID_DIGGER_DEATH     33
#define PXID_EXIT             34
#define PXID_CHANGER          35

#define PXID_GHOST_DOWN_180   36
#define PXID_GHOST_UP_180     37
#define PXID_GHOST_LEFT_180   38
#define PXID_GHOST_RIGHT_180  39
#define PXID_GHOST_DOWN_L     40
#define PXID_GHOST_UP_L       41
#define PXID_GHOST_LEFT_L     42
#define PXID_GHOST_RIGHT_L    43
#define PXID_GHOST_DOWN_R     44
#define PXID_GHOST_UP_R       45
#define PXID_GHOST_LEFT_R     46
#define PXID_GHOST_RIGHT_R    47
#define PXID_GHOST_DOWN_LR    48
#define PXID_GHOST_UP_LR      49
#define PXID_GHOST_LEFT_LR    50
#define PXID_GHOST_RIGHT_LR   51
#define CountPixmaps          52

#define PXID_ICON_NOTHING      0
#define PXID_ICON_DIGGER       1
#define PXID_ICON_DIGGER_FOOT  2
#define PXID_ICON_DIGGER_EYE1  3
#define PXID_ICON_DIGGER_EYE2  4
#define CountIconPixmaps       5

extern Pixmap /*main_win_icon_pixmap,*/ pixmaps[], iconpixmaps[], xbm_charset[];
extern Cursor cursor_digger, cursor_watch;

extern void Make16x16Pixmaps_Editor();
extern void MakeXPixmaps(Bool first_time);
extern void CopyOnePixmap_Window(Window window, unsigned char pixmap_id,
                                 int x, int y);
extern void DrawOnePixmap(unsigned char pixmap_id, int x, int y);
extern void DrawPixmapStr(const char *str, int x, int y);
extern void ResizeXPixmaps(int newmag);

#endif
