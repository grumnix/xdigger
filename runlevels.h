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

#ifndef _RUNLEVELS_H
#define _RUNLEVELS_H

#include "typedef.h"

#define MAX_GHOSTS 16

extern FieldEntry field[20][14];
extern Bool intakt;
extern int score;
extern Bool ones_cheated;
extern Bool in_runlevels;
extern Bool in_LevelEditor;
extern int fdiggeridlesequence[2][13];

extern Bool DoTakt();
extern void RunLevels(int von_level, int bis_level);
extern void ALookAtTheRooms();
extern void LevelEditor();
extern unsigned char id2pixmap_id(int id, int flags);

#endif
