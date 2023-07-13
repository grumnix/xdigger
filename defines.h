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

#ifndef _DEFINES_H

#define _DEFINES_H

#define ID_NOTHING    0
#define ID_STONE      1
#define ID_GROUND     2
#define ID_GHOST      3
#define ID_LDIGGER    4
#define ID_DIAMOND    5
#define ID_WALL       6
#define ID_FSTODMD    8
#define ID_IVSTONE    9
#define ID_DIGGER    10
#define ID_EXIT      12
#define ID_IVDIAMOND 13
#define ID_CHANGER   14

#define F_DIGGER_MOVEMASK 0x1f
#define F_DIGGER_DOWN0    0x00
#define F_DIGGER_LEFT0    0x05
#define F_DIGGER_LEFT1    0x06
#define F_DIGGER_LEFT2    0x07
#define F_DIGGER_LEFT3    0x08
#define F_DIGGER_RIGHT0   0x09
#define F_DIGGER_RIGHT1   0x0a
#define F_DIGGER_RIGHT2   0x0b
#define F_DIGGER_RIGHT3   0x0c
#define F_DIGGER_UP1      0x0d
#define F_DIGGER_UP2      0x0e
#define F_DIGGER_DOWN1    0x0f
#define F_DIGGER_DOWN2    0x10
#define F_DIGGER_FOOT     0x11
#define F_DIGGER_EYE1     0x12
#define F_DIGGER_EYE2     0x13
#define F_DIGGER_DEATH    0x80

#define F_GHOST_MOVEMASK  0x03
#define F_GHOST_DOWN      0x00
#define F_GHOST_UP        0x01
#define F_GHOST_RIGHT     0x02
#define F_GHOST_LEFT      0x03
#define F_GHOST_TYPEMASK  0x1c
#define F_GHOST_NOTYPE    0x00
#define F_GHOST_TYPE180   0x04
#define F_GHOST_TYPEL     0x08
#define F_GHOST_TYPER     0x0c
#define F_GHOST_TYPELR    0x10

#define F_DIAMOND_BLINKMASK 0x0f
#define F_DIAMOND_NOBLINK   0x00
#define F_DIAMOND_BLINK0    0x01
#define F_DIAMOND_BLINK1    0x02
#define F_DIAMOND_BLINK2    0x03
#define F_DIAMOND_BLINK3    0x04
#define F_DIAMOND_BLINK4    0x05
#define F_DIAMOND_BLINK5    0x06
#define F_DIAMOND_BLINK6    0x07
#define F_DIAMOND_BLINK7    0x08

#define ADIGGERN 0
#define ADIGGERL 1
#define ADIGGERR 2
#define ADIGGERU 3
#define ADIGGERD 4

#define AGHOSTD 0      /* lastarrow down */
#define AGHOSTU 1
#define AGHOSTR 2
#define AGHOSTL 3
#define TGHOST180  0   /* ghosttype 180° */
#define TGHOST90L  1
#define TGHOST90LR 2
#define TGHOST90R  3
#define LGHOSTL 0      /* lastturn left  */
#define LGHOSTR 1      /* lastturn right */

#endif /* _DEFINES_H */
