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
#include <sys/time.h>
#include <unistd.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "configure.h"
#include "defines.h"
#include "drawpixmaps.h"
#include "drawtext.h"
#include "icon.h"
#include "runlevels.h"
#include "sound.h"
#include "typedef.h"
#include "xdigger.h"

FieldEntry field[20][14];
GhostEntry ghosts[16];
int akt_level_number, new_level_number, lives, wait_count;
Bool last_level = False;
int number_diamonds, collected_diamonds, collected_grounds, number_ghosts;
int lefttime, score;
int diggerposx, diggerposy;
int ldiggerposx = -1, ldiggerposy = -1;
Bool levelready, alllevelsready, digger_death, digger_candie = True;
Bool intakt = False;
Bool inpause = False, step = False;
int lastdiggerarrow;
Bool in_runlevels = False;
Bool in_LevelEditor = False;
Bool indiggeridlesequence;
Bool cheat = False, ones_cheated = False;
Bool ton_schritt, ton_steine, ton_diamant;
Bool replay = False;

void ChangePS()
{
  char slevel[3], scmdln[7];

  sprintf(slevel, "%d", akt_level_number);
  if (cheat)
    strcat(strcat(strcpy(scmdln, " (C"), slevel), ")");
  else
    strcat(strcat(strcpy(scmdln, " (L"), slevel), ")");
  strcpy(LastArgv, scmdln);
} /* ChangePS() */

unsigned char id2pixmap_id(int id, int flags)
{
  switch (id)
  {
    case ID_NOTHING : return(PXID_NOTHING);
    case ID_STONE : return(PXID_STONE);
    case ID_GROUND : return(PXID_GROUND);
    case ID_GHOST : 
      {
        switch (flags & F_GHOST_TYPEMASK)
        {          
        case F_GHOST_NOTYPE:
          switch (flags & F_GHOST_MOVEMASK)
          {
          case F_GHOST_DOWN:  return(PXID_GHOST);
          case F_GHOST_UP:    return(PXID_GHOST_UP);
          case F_GHOST_LEFT:  return(PXID_GHOST_LEFT);
          case F_GHOST_RIGHT: return(PXID_GHOST_RIGHT);
          } 
        case F_GHOST_TYPE180:
          switch (flags & F_GHOST_MOVEMASK)
          {
          case F_GHOST_DOWN:  return(PXID_GHOST_DOWN_180);
          case F_GHOST_UP:    return(PXID_GHOST_UP_180);
          case F_GHOST_LEFT:  return(PXID_GHOST_LEFT_180);
          case F_GHOST_RIGHT: return(PXID_GHOST_RIGHT_180);
          } 
        case F_GHOST_TYPEL:
          switch (flags & F_GHOST_MOVEMASK)
          {
          case F_GHOST_DOWN:  return(PXID_GHOST_DOWN_L);
          case F_GHOST_UP:    return(PXID_GHOST_UP_L);
          case F_GHOST_LEFT:  return(PXID_GHOST_LEFT_L);
          case F_GHOST_RIGHT: return(PXID_GHOST_RIGHT_L);
          } 
        case F_GHOST_TYPER:
          switch (flags & F_GHOST_MOVEMASK)
          {
          case F_GHOST_DOWN:  return(PXID_GHOST_DOWN_R);
          case F_GHOST_UP:    return(PXID_GHOST_UP_R);
          case F_GHOST_LEFT:  return(PXID_GHOST_LEFT_R);
          case F_GHOST_RIGHT: return(PXID_GHOST_RIGHT_R);
          }
        case F_GHOST_TYPELR:          
          switch (flags & F_GHOST_MOVEMASK)
          {
          case F_GHOST_DOWN:  return(PXID_GHOST_DOWN_LR);
          case F_GHOST_UP:    return(PXID_GHOST_UP_LR);
          case F_GHOST_LEFT:  return(PXID_GHOST_LEFT_LR);
          case F_GHOST_RIGHT: return(PXID_GHOST_RIGHT_LR);
          }
        }        
      }
    case ID_DIAMOND : /*return(PXID_DIAMOND);*/
      {
        switch (flags & F_DIAMOND_BLINKMASK)
        {
          case F_DIAMOND_NOBLINK:  return(PXID_DIAMOND);
          case F_DIAMOND_BLINK0:   return(PXID_DIAMOND_BLINK0);
          case F_DIAMOND_BLINK1:   return(PXID_DIAMOND_BLINK1);
          case F_DIAMOND_BLINK2:   return(PXID_DIAMOND_BLINK2);
          case F_DIAMOND_BLINK3:   return(PXID_DIAMOND_BLINK3);
          case F_DIAMOND_BLINK4:   return(PXID_DIAMOND_BLINK4);
          case F_DIAMOND_BLINK5:   return(PXID_DIAMOND_BLINK5);
        }
      }
    case ID_WALL : return(PXID_WALL);
    case ID_DIGGER : 
      {
	if ((flags & F_DIGGER_DEATH) == 0)
	  switch (flags & F_DIGGER_MOVEMASK)
	    {
	    case F_DIGGER_LEFT0:  return(PXID_DIGGER_LEFT0);
	    case F_DIGGER_LEFT1:  return(PXID_DIGGER_LEFT1);
	    case F_DIGGER_LEFT2:  return(PXID_DIGGER_LEFT2);
	    case F_DIGGER_LEFT3:  return(PXID_DIGGER_LEFT3);
	    case F_DIGGER_RIGHT0: return(PXID_DIGGER_RIGHT0);
	    case F_DIGGER_RIGHT1: return(PXID_DIGGER_RIGHT1);
	    case F_DIGGER_RIGHT2: return(PXID_DIGGER_RIGHT2);
	    case F_DIGGER_RIGHT3: return(PXID_DIGGER_RIGHT3);
	    case F_DIGGER_UP1:    return(PXID_DIGGER_UP1);
	    case F_DIGGER_UP2:    return(PXID_DIGGER_UP2);
	    case F_DIGGER_DOWN0:  return(PXID_DIGGER);
	    case F_DIGGER_DOWN1:  return(PXID_DIGGER_DOWN1);
	    case F_DIGGER_DOWN2:  return(PXID_DIGGER_DOWN2);
	    case F_DIGGER_FOOT:   return(PXID_DIGGER_FOOT);
	    case F_DIGGER_EYE1:   return(PXID_DIGGER_EYE1);
	    case F_DIGGER_EYE2:   return(PXID_DIGGER_EYE2);
	    } 
	else return(PXID_DIGGER_DEATH);
      }
    case ID_EXIT : return(PXID_EXIT);
    case ID_CHANGER : return(PXID_CHANGER);
    }
  return(PXID_NOTHING);
} /* unsigned char id2pixmap_id(int id, int flags) */

void CopyField(FieldEntry *dest, FieldEntry *source)
{
  dest->id        = source->id;
  dest->flags     = source->flags;
  dest->pixmap_id = source->pixmap_id;
  /*dest->todraw    = True; ???*/
} /* CopyField(FieldEntry *dest, FieldEntry *source) */

void FillField(FieldEntry *fieldentry, unsigned char id, unsigned char flags)
{
  fieldentry->id = id;
  fieldentry->flags = flags;
  fieldentry->pixmap_id = id2pixmap_id(id, flags);
  fieldentry->todraw = True;
} /* FillField(FieldEntry *fieldentry, unsigned char id, unsigned char flags)*/

void FillFieldNothing(FieldEntry *fieldentry)
{
  fieldentry->id        = ID_NOTHING;
  fieldentry->flags     = 0;
  fieldentry->pixmap_id = id2pixmap_id(ID_NOTHING, 0);
} /* FillFieldNothing(FieldEntry *fieldentry) */

Bool Test_ReadWrite(char *filename)
{
  FILE *filehandle;
  
  if ((filehandle = fopen(filename, "r+")) != NULL)
  {
    fclose(filehandle);
    return(True);
  }
  else return(False);
} /* Bool Test_ReadWrite(char *filename) */

Bool LoadLevel(int level_number, Bool edit)
{
/*   char filename[256]; */
  unsigned char file_daten[0x9c], *c, ginfo, b;
  int x, y, id, flags;
  FILE *filehandle;

/*   XDefineCursor(display, mainwindow, cursor_watch); */
    
  akt_level_number = level_number;
  new_level_number = 0;
  number_ghosts = 0;
  
  if ((filehandle = fopen(level_filename, "r")) == NULL) return(False);
  
  if ((edit) && !(Test_ReadWrite(level_filename)))
  {
    fclose(filehandle);
    fprintf(stderr, "%s: can't open %s for rw\n", progname, level_filename);
    return(False);
  }

  fseek(filehandle, (level_number -1 ) * 0x9c, SEEK_SET);
  fread(file_daten, 0x9c, 1, filehandle);
  fclose(filehandle);
  
  c = file_daten;
  for (y=0; y<14; y++) for (x=0; x<20; x++)
    {
      if (!(x & 1))
        id = *c >> 4; 
      else
      {
        id = *c & 0xf;
        c++;
      }

      flags = 0;
      
      if ((id == 3) || (id == 7) || (id == 11) || (id == 15))
	{
	  ginfo = file_daten[0x94 + number_ghosts/2];
	  if (number_ghosts & 1) ginfo = ginfo & 0xf; else ginfo = ginfo >> 4;
	  ghosts[number_ghosts].x = x;
	  ghosts[number_ghosts].y = y;
	  ghosts[number_ghosts].type = id >> 2;
	  ghosts[number_ghosts].arrow = ginfo;
	  ghosts[number_ghosts].lastturn = LGHOSTL;
	  ghosts[number_ghosts].dead = False;
          if (edit)
          {
            switch (id)
            {
            case 3:
              flags = F_GHOST_TYPE180;
              break;      
            case 7:
              flags = F_GHOST_TYPEL;
              break;     
            case 11:
              flags = F_GHOST_TYPELR;
              break;    
            case 15:
              flags = F_GHOST_TYPER;
              break;              
            }
            switch (ginfo)
            {
            case AGHOSTL:
              flags |= F_GHOST_LEFT;
              break;
            case AGHOSTR:
              flags |= F_GHOST_RIGHT;
              break;
            case AGHOSTU:
              flags |= F_GHOST_UP;
              break;
            case AGHOSTD:
              flags |= F_GHOST_DOWN;
              break;
            }            
          }
          id = 3;
	  number_ghosts++;
	}

      field[x][y].id         = id;
      field[x][y].flags      = flags;
      field[x][y].pixmap_id  = 
	id2pixmap_id(field[x][y].id, field[x][y].flags);
      field[x][y].todraw     = True;
    }
  c += 3; 
  last_level = ((*c == 0) && (*(c+1) == 0));
  c += 2;
  diggerposx = *(c++);
  diggerposy = *(c++)-2;
  b = *(c++);
  number_diamonds = (b/0x10)*10 + (b%0x10);
  ldiggerposx = -1; ldiggerposy = -1;
  levelready = False;
  digger_death = False;
  lastdiggerarrow = ADIGGERN;
  indiggeridlesequence = False;
  collected_diamonds = 0;
  collected_grounds = 0;
  lefttime = 5000;
  if (!replay) wait_count = 256;
/*   XDefineCursor(display, mainwindow, cursor_digger); */
  return(True);
} /* loadlevel(int level_number, Bool edit) */

void DrawField()
{
  int x, y;

  for (y=0; y<14; y++) for (x=0; x<20; x++)
    if (field[x][y].todraw)
      {
	DrawOnePixmap(field[x][y].pixmap_id, x, y);
	field[x][y].todraw = False;
      }
} /* DrawField() */

void Restore_LeftTime()
{
  char slefttime[7];

  sprintf(slefttime, "%.6d", lefttime);
  if ((lefttime < 1000) && ((lefttime % 4) <= 1) && (lefttime != 0))
    strcpy(slefttime, "      ");
  WriteTextStr(slefttime, 18, vertvar, kcf_weiss, kcb_rot);
} /* Restore_LeftTime() */

void Restore_Diamonds()
{
  char snumber_diamonds[3];

  sprintf(snumber_diamonds, "%.2d", number_diamonds);
  WriteTextStr(snumber_diamonds, 36, vertvar, kcf_weiss, kcb_rot);
} /* Restore_Diamonds() */

void Restore_Score()
{
  char sscore[7];

  sprintf(sscore, "%.6d", score);
  WriteTextStr(sscore, 18, 1+vertvar, kcf_weiss, kcb_rot);
} /* Restore_Score() */

void Restore_Collected_Diamonds()
{
  char scollected_diamonds[3];

  sprintf(scollected_diamonds, "%.2d", collected_diamonds);
  WriteTextStr(scollected_diamonds, 36, 1+vertvar, kcf_weiss, kcb_rot);
} /* Restore_Collected_Diamonds() */

void Init_First_Line()
{
  char croom[41], clives[41], slevel_number[3], slives[20];

  sprintf(slevel_number, "%.2d", akt_level_number);
  sprintf(slives, "%.2d", lives);
  strcat(strcpy(croom, " ROOM:  "), slevel_number);
  strcat(strcpy(clives, " LIVES: "), slives);

  if (!vert240)
    WriteTextStr("\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135", 0, 0, kcf_tuerkis, kcb_blau);

  WriteTextStr(croom, 0, vertvar, kcf_weiss, kcb_rot);
  WriteTextStr(" TIME:  ", 10, vertvar, kcf_weiss, kcb_rot);
  Restore_LeftTime();
  WriteTextStr(" DIAMONDS:  ", 24, vertvar, kcf_weiss, kcb_rot);
  Restore_Diamonds();
  WriteTextStr("  ", 38, vertvar, kcf_weiss, kcb_rot);

  WriteTextStr(clives, 0, 1+vertvar, kcf_weiss, kcb_rot);
  Restore_Collected_Diamonds();
  WriteTextStr(" SCORE: ", 10, 1+vertvar, kcf_weiss, kcb_rot);
  WriteTextStr(" COLLECTED: ", 24, 1+vertvar, kcf_weiss, kcb_rot);
  Restore_Score();
  WriteTextStr("  ", 38, 1+vertvar, kcf_weiss, kcb_rot);
  if (!vert240)
    WriteTextStr("\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135\135", 0, 3, kcf_tuerkis, kcb_blau);

} /* Init_First_Line() */

void Handle_ExposeEvent(XExposeEvent *xexpose)
{
  int x, xa, xe, y, ya, ye;
  XWindowAttributes windowattributes;

  /* Resize ? */
  XGetWindowAttributes(display, mainwindow, &windowattributes);
  if (xpmmag != windowattributes.width / 320)
    ResizeXPixmaps(windowattributes.width / 320);
  
  if (xexpose->y < (vertvar+1)*16 * xpmmag)
    /*restore_first_line = True;*/
    Mark_CharField(xexpose, 1+2*vertvar);

  xa = (xexpose->x / (16*xpmmag));
  xe = ((xexpose->x + xexpose->width - 1) / (16*xpmmag));
  if (xa > 19) return;
  if (xe > 19) xe = 19;
  ya = (xexpose->y / (16*xpmmag)) - (1+vertvar); if (ya < 0) ya = 0;
  ye = ((xexpose->y + xexpose->height - 1) / (16*xpmmag)) - (1+vertvar);
  if (ya > 13) return;
  if (ye > 13) ye = 13;
  if (ye < 0) ye = 0;
  for (y=ya; y<=ye; y++) for (x=xa; x<=xe; x++)
    field[x][y].todraw = True; 
  if (xexpose->count == 0)
    {
      DrawTextField();
      DrawField();
    }
} /* Handle_ExposeEvent(XExposeEvent *xexpose) */

void DieDigger()
{
  if ((digger_candie) && (!digger_death))
    {
      digger_death = True;
      field[diggerposx][diggerposy].flags = F_DIGGER_DEATH;
      field[diggerposx][diggerposy].pixmap_id = 
	id2pixmap_id(ID_DIGGER, F_DIGGER_DEATH);
      field[diggerposx][diggerposy].todraw = True;
    }
} /* DieDigger() */

Bool NoGroundsLeft()
{
  int x, y;

  for (y=0; y<14; y++) for (x=0; x<20; x++)
    if (field[x][y].id == ID_GROUND) return(False);
  return(True);
} /* NoGroundsLeft() */

Bool kb_left, kb_right, kb_up, kb_down;
Bool stone_r = False, stone_l = False;

int fdiggerleftsequence[6] = 
{F_DIGGER_LEFT1, F_DIGGER_LEFT2, F_DIGGER_LEFT3, F_DIGGER_LEFT2, 
 F_DIGGER_LEFT1, F_DIGGER_LEFT0};
int fdiggerrightsequence[6] = 
{F_DIGGER_RIGHT1, F_DIGGER_RIGHT2, F_DIGGER_RIGHT3, F_DIGGER_RIGHT2, 
 F_DIGGER_RIGHT1, F_DIGGER_RIGHT0};
int fdiggerupsequence[2] = {F_DIGGER_UP1, F_DIGGER_UP2};
int fdiggerdownsequence[2] = {F_DIGGER_DOWN1, F_DIGGER_DOWN2};
int fdiggeridlesequence[2][13] =
{{10, F_DIGGER_FOOT, F_DIGGER_FOOT, F_DIGGER_DOWN0, F_DIGGER_DOWN0, 
  F_DIGGER_FOOT, F_DIGGER_FOOT, F_DIGGER_DOWN0, F_DIGGER_DOWN0, 
  F_DIGGER_FOOT, F_DIGGER_FOOT, F_DIGGER_DOWN0, F_DIGGER_DOWN0},
 {8, F_DIGGER_EYE1, F_DIGGER_EYE2, F_DIGGER_EYE1, F_DIGGER_DOWN0,
  F_DIGGER_EYE1, F_DIGGER_EYE2, F_DIGGER_EYE1, F_DIGGER_DOWN0, 0, 0}};

int fdiggersequenceidx, fdiggeridleidx;

void DiggerStep()
{
  int dx, dy, zx, zy;
  int arrow, pixid;

  /* Loesche alte Schattenposition */
  if ((ldiggerposx >= 0) && (ldiggerposy >= 0))
    {
      field[ldiggerposx][ldiggerposy].id = ID_NOTHING;
      ldiggerposx = -1;
      ldiggerposy = -1;
    }

  if (digger_death) return;
  dx = diggerposx; dy = diggerposy; zx = dx; zy = dy;
  if (kb_left) {zx--; arrow = ADIGGERL;}
  else if (kb_right) {zx++; arrow = ADIGGERR;}
  else if (kb_up) {zy--; arrow = ADIGGERU;}
  else if (kb_down) {zy++; arrow = ADIGGERD;}
  else arrow = ADIGGERN;
  
  if (stone_r && !kb_right) stone_r = False;
  if (stone_l && !kb_left)  stone_l = False;
  
  if ((zx != dx) || (zy != dy))
      {
	if (field[zx][zy].id == ID_DIAMOND) 
	  {
	    collected_diamonds++;
	    Restore_Collected_Diamonds();
	    score = score + 3;
	    Restore_Score();
	    ton_diamant = True;
	  }
	if ((field[zx][zy].id == ID_GROUND) && laszlo)
	  {
	    collected_grounds++;
	    if (collected_grounds == 10)
	      {
		collected_grounds = 0;
		score++;
		Restore_Score();
	      }
	  }
	if (field[zx][zy].id == ID_STONE)
	  {
	    if ((zx>dx) && (field[zx+1][zy].id == ID_NOTHING))
		{
		  if (stone_r)
		    {
		      CopyField(&field[dx+2][dy], &field[dx+1][dy]);
		      field[dx+2][dy].todraw = True;

		      field[dx+1][dy].id = ID_NOTHING;
		      field[dx+1][dy].flags = 0;
		      field[dx+1][dy].pixmap_id = id2pixmap_id(ID_NOTHING, 0);
		      field[dx+1][dy].todraw = True;

		      stone_r = False;
		    }
		  else stone_r = True;
		}
	    if ((zx<dx) && (field[zx-1][zy].id == ID_NOTHING))
		{
		  if (stone_l)
		    {
		      CopyField(&field[dx-2][dy], &field[dx-1][dy]);
		      field[dx-2][dy].todraw = True;

		      field[dx-1][dy].id = ID_NOTHING;
		      field[dx-1][dy].flags = 0;
		      field[dx-1][dy].pixmap_id = id2pixmap_id(ID_NOTHING, 0);
		      field[dx-1][dy].todraw = True;

		      stone_l = False;
		    }
		  else stone_l = True;
		}
	  }
	if ((field[zx][zy].id == ID_NOTHING) ||
	    (field[zx][zy].id == ID_GROUND) ||
	    (field[zx][zy].id == ID_DIAMOND))
	  {
	    CopyField(&field[zx][zy], &field[dx][dy]);
	    field[zx][zy].todraw    = True;

	    field[dx][dy].id        = ID_LDIGGER;
	    field[dx][dy].flags     = 0;
	    field[dx][dy].pixmap_id = id2pixmap_id(ID_NOTHING, 0);
	    field[dx][dy].todraw    = True;

	    diggerposx  = zx;
	    diggerposy  = zy;
	    ldiggerposx = dx;
	    ldiggerposy = dy;

	    ton_schritt = True;
	  }
	if ((field[zx][zy].id == ID_EXIT) && 
	    (collected_diamonds >= number_diamonds))
	  {
	    if ((laszlo) && NoGroundsLeft())
	      {
		score = score + 100;
		Restore_Score();
	      }
	    levelready = True;
	  }
	if (field[zx][zy].id == ID_GHOST) DieDigger();
      }

  if (!digger_death)
    {
      dx = diggerposx;
      dy = diggerposy;
      pixid = field[dx][dy].pixmap_id;
      switch (arrow)
	{
	case ADIGGERL:
	  if (lastdiggerarrow == ADIGGERL)
	    {if (++fdiggersequenceidx == 6) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;	  
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerleftsequence[fdiggersequenceidx];
	  break;
	case ADIGGERR:
	  if (lastdiggerarrow == ADIGGERR)
	    {if (++fdiggersequenceidx == 6) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerrightsequence[fdiggersequenceidx];
	  break;
	case ADIGGERU:
	  if (lastdiggerarrow == ADIGGERU)
	    {if (++fdiggersequenceidx == 2) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerupsequence[fdiggersequenceidx];
	  break;
	case ADIGGERD:
	  if (lastdiggerarrow == ADIGGERD)
	    {if (++fdiggersequenceidx == 2) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerdownsequence[fdiggersequenceidx];
	  break;
	case ADIGGERN:
	  if (lastdiggerarrow == ADIGGERN)
	    {
	      if ((!indiggeridlesequence) && (MyRand(10) == 1))
		{
		  fdiggeridleidx = MyRand(2);
		  fdiggersequenceidx = 0;
		  indiggeridlesequence = True;
		}
	      if (indiggeridlesequence)
		{
		  if (++fdiggersequenceidx > 
		      fdiggeridlesequence[fdiggeridleidx][0])
		    indiggeridlesequence = False;
		}
	    }
	  else indiggeridlesequence = False;
	  if (indiggeridlesequence)
	    field[dx][dy].flags =
	      (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	      fdiggeridlesequence[fdiggeridleidx][fdiggersequenceidx];
	  else
	    field[dx][dy].flags =
	      (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | F_DIGGER_DOWN0;
	  break;
	}
      field[dx][dy].pixmap_id = 
	id2pixmap_id(field[dx][dy].id, field[dx][dy].flags);
      if (field[dx][dy].pixmap_id != pixid) field[dx][dy].todraw = True;
      lastdiggerarrow = arrow;
    }
} /* DiggerStep() */

void DiggerHalfStep()
{
  int dx, dy;
  int arrow, pixid;

  if (!digger_death)
    {
      dx = diggerposx;
      dy = diggerposy;
      pixid = field[dx][dy].pixmap_id;
      arrow = lastdiggerarrow;
      switch (arrow)
	{
	case ADIGGERL:
	  if (lastdiggerarrow == ADIGGERL)
	    {if (++fdiggersequenceidx == 6) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;	  
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerleftsequence[fdiggersequenceidx];
	  break;
	case ADIGGERR:
	  if (lastdiggerarrow == ADIGGERR)
	    {if (++fdiggersequenceidx == 6) fdiggersequenceidx = 0;}
	  else fdiggersequenceidx = 0;
	  field[dx][dy].flags = (field[dx][dy].flags & !F_DIGGER_MOVEMASK) | 
	    fdiggerrightsequence[fdiggersequenceidx];
	  break;
	}
      field[dx][dy].pixmap_id = 
	id2pixmap_id(field[dx][dy].id, field[dx][dy].flags);
      if (field[dx][dy].pixmap_id != pixid) field[dx][dy].todraw = True;
      lastdiggerarrow = arrow;
    }
} /* DiggerHalfStep() */

void StoneDiamondStep()
{

#define SF_DOWN      0
#define SF_RIGHTDOWN 1
#define SF_LEFTDOWN  2
#define SF_CHANGER   3
  
  typedef struct
  {
    unsigned char x, y, id, type;
  } TMerk;
  
  TMerk merk[140], ghosts_todie[17];
  int i, x, y, dx, dy, merkidx, ghosts_todie_idx;
  Bool digger_todie = False;  
  
  void Merke(unsigned char type)
    {
      merk[merkidx].x = x;
      merk[merkidx].y = y;
      merk[merkidx].id = field[x][y].id & 7;
      merk[merkidx].type = type;
      merkidx++;   
    }  
  
  /* Schritt 1  (nur markieren) : */
  merkidx = 0; ghosts_todie_idx = 0;
  
  for (y=0; y<14; y++) for (x=0; x<20; x++)
    if ((field[x][y].id == ID_STONE) ||
	(field[x][y].id == ID_DIAMOND) ||
	(field[x][y].id == ID_IVSTONE) ||
        (field[x][y].id == ID_IVDIAMOND))
    {
      if (field[x][y+1].id == ID_NOTHING)
        Merke(SF_DOWN);
      else
        if ((field[x][y+1].id == ID_STONE) ||
            (field[x][y+1].id == ID_DIAMOND))
        {
          if ((field[x-1][y].id == ID_NOTHING) &&
              (field[x-1][y+1].id == ID_NOTHING))
            Merke(SF_LEFTDOWN);
          else
            if ((field[x+1][y].id == ID_NOTHING) &&
                (field[x+1][y+1].id == ID_NOTHING))
            {          
              Merke(SF_RIGHTDOWN);
              field[x+1][y+1].id = ID_FSTODMD;
            }        
        }
        else
          if (((field[x][y].id == ID_STONE) ||
               (field[x][y].id == ID_IVSTONE)) &&
              (field[x][y+1].id == ID_CHANGER) &&
              (field[x][y+2].id == ID_NOTHING))
            Merke(SF_CHANGER);
    }
  merk[merkidx].x = 0;
  merk[merkidx].y = 0;

  /* Schritt 2  (wirklich bewegen) : */
  merkidx = 0;
  while ((merk[merkidx].x != 0) || (merk[merkidx].y != 0))
  {
    x = merk[merkidx].x;
    y = merk[merkidx].y;
    switch (merk[merkidx].type)
    {
    case SF_DOWN:
      dx = x;
      dy = y+1;
      break;
    case SF_LEFTDOWN:
      dx = x-1;
      dy = y+1;
      break;
    case SF_RIGHTDOWN:
      dx = x+1;
      dy = y+1;
      break;
    case SF_CHANGER:
      dx = x;
      dy = y+2;
      break;
    }
    if ((dy - y) == 2) /* Umwandler*/
      FillField(&field[dx][dy], ID_DIAMOND, 0);
    else
      if ((field[x][y].id == ID_STONE) ||
          (field[x][y].id == ID_IVSTONE))
        FillField(&field[dx][dy], ID_STONE, 0);
      else
        if ((field[x][y].id == ID_DIAMOND) ||
            (field[x][y].id == ID_IVDIAMOND))
          FillField(&field[dx][dy], ID_DIAMOND, 0);
    FillField(&field[x][y], ID_NOTHING, 0);
    if (field[dx][dy+1].id == ID_DIGGER) digger_todie = True;
    if (field[dx][dy+1].id == ID_GHOST)
    {
      /* Geist von Stein/Diamant getroffen --> Sprengen */
      ghosts_todie[ghosts_todie_idx].x = dx;
      ghosts_todie[ghosts_todie_idx].y = dy+1;
      ghosts_todie_idx++;      
    }
    
    if ((field[dx][dy+1].id == ID_STONE) ||
        (field[dx][dy+1].id == ID_DIAMOND) ||
        (field[dx][dy+1].id == ID_WALL) ||
        (field[dx][dy+1].id == ID_GHOST))
      ton_steine = True;    
    merkidx++;
  }

  ghosts_todie[ghosts_todie_idx].x = 0;
  ghosts_todie[ghosts_todie_idx].y = 0;

  /* Digger vom Stein/Diamant getroffen --> TOT! */
  if (digger_todie) DieDigger();

  /* Geister toeten */
  ghosts_todie_idx = 0;  
  while ((ghosts_todie[ghosts_todie_idx].x != 0) ||
         (ghosts_todie[ghosts_todie_idx].y != 0))    
  {
    for (y=ghosts_todie[ghosts_todie_idx].y-1;
         y<=ghosts_todie[ghosts_todie_idx].y+1; y++)
      for (x=ghosts_todie[ghosts_todie_idx].x-1;
           x<=ghosts_todie[ghosts_todie_idx].x+1; x++)
        if ((x>0) && (x<19) && (y>0) && (y<13))
        {
          if (field[x][y].id == ID_DIGGER) DieDigger();
          else
          {
            if (field[x][y].id == ID_GHOST)
              for (i=0; i<number_ghosts; i++)
                if ((ghosts[i].x == x) && (ghosts[i].y == y))
                {
                  ghosts[i].dead = True;
                  ghosts[i].x = 100;
                  ghosts[i].y = 100;
                  score = score + 99;
                  Restore_Score();
                  break;
                }
            FillField(&field[x][y], ID_NOTHING, 0);
          }
        }
    ghosts_todie_idx++;    
  }  
} /* StoneDiamondStep() */

int fdiamondblinksequence[6] = 
/*{F_DIAMOND_BLINK0, F_DIAMOND_BLINK1, F_DIAMOND_BLINK2, F_DIAMOND_BLINK3,
 F_DIAMOND_BLINK4, F_DIAMOND_BLINK5, F_DIAMOND_BLINK6, F_DIAMOND_BLINK7};*/
{F_DIAMOND_BLINK5, F_DIAMOND_BLINK4, F_DIAMOND_BLINK3, F_DIAMOND_BLINK2,
 F_DIAMOND_BLINK1, F_DIAMOND_BLINK0};

int fdiamondblinksequenceidx;

void DiamondBlinkStep()
{
  int x, y;

  if (++fdiamondblinksequenceidx == 6) fdiamondblinksequenceidx = 0;
  for (y=0; y<=13; y++) for (x=0; x<=19; x++)
  {
    if (field[x][y].id == ID_DIAMOND)
    {
      field[x][y].flags = (field[x][y].flags & !F_DIAMOND_BLINKMASK) | 
        fdiamondblinksequence[(fdiamondblinksequenceidx+4*y)%6];
      field[x][y].pixmap_id = 
	id2pixmap_id(field[x][y].id, field[x][y].flags);
      field[x][y].todraw = True;
    }
  }
  
} /* DiamondBlinkStep() */

void GhostStep()
{
  int g, i, x, y, wx[4], wy[4], dx, dy, arrow, larrow, lastturn;

  for (g=0; g < number_ghosts; g++) if (!ghosts[g].dead)
    {
      x = ghosts[g].x; wx[3] = wx[2] = wx[1] = wx[0] = x;
      y = ghosts[g].y; wy[3] = wy[2] = wy[1] = wy[0] = y;
      arrow = ghosts[g].arrow;
      switch (ghosts[g].type)
	{
	case TGHOST180:
	  if (arrow == AGHOSTL) {wx[0]--; wx[1]++;} else
	    if (arrow == AGHOSTR) {wx[0]++; wx[1]--;} else
	      if (arrow == AGHOSTU) {wy[0]--; wy[1]++;} else
		if (arrow == AGHOSTD) {wy[0]++; wy[1]--;}
	  break;
	case TGHOST90L:
	  if (arrow == AGHOSTL) 
	    {wx[0]--; wy[1]++; wy[2]--; wx[3]++;} else
	      if (arrow == AGHOSTR) 
		{wx[0]++; wy[1]--; wy[2]++; wx[3]--;} else
		  if (arrow == AGHOSTU) 
		    {wy[0]--; wx[1]--; wx[2]++; wy[3]++;} else
		      if (arrow == AGHOSTD) 
			{wy[0]++; wx[1]++; wx[2]--; wy[3]--;}
	  break;
	case TGHOST90R:
	  if (arrow == AGHOSTL)
	    {wx[0]--; wy[1]--; wy[2]++; wx[3]++;} else
	      if (arrow == AGHOSTR) 
		{wx[0]++; wy[1]++; wy[2]--; wx[3]--;} else
		  if (arrow == AGHOSTU) 
		    {wy[0]--; wx[1]++; wx[2]--; wy[3]++;} else
		      if (arrow == AGHOSTD) 
			{wy[0]++; wx[1]--; wx[2]++; wy[3]--;}
	  break;

	case TGHOST90LR:
	  lastturn = ghosts[g].lastturn;
	  if (arrow == AGHOSTL)
	    {
	      wx[0]--; 
	      if (lastturn == LGHOSTL) {wy[1]--; wy[2]++;} 
	      else {wy[1]++; wy[2]--;}
	      wx[3]++;
	    }
	  else
	    if (arrow == AGHOSTR)
	      {
		wx[0]++; 
		if (lastturn == LGHOSTL) {wy[1]++; wy[2]--;} 
		else {wy[1]--; wy[2]++;}
		wx[3]--;
	      }
	    else
	      if (arrow == AGHOSTU)
		{
		  wy[0]--; 
		  if (lastturn == LGHOSTL) {wx[1]++; wx[2]--;} 
		  else {wx[1]--; wx[2]++;}
		  wy[3]++;
		}
	      else
	      if (arrow == AGHOSTD)
		{
		  wy[0]++; 
		  if (lastturn == LGHOSTL) {wx[1]--; wx[2]++;} 
		  else {wx[1]++; wx[2]--;}
		  wy[3]--;
		}
	  break;
	}
      /* ------------------------------------------------------------------ */
      for (i=0; i<4; i++) 
	if ((wx[i] != x) || (wy[i] != y))
	  {
	    dx = wx[i]; dy = wy[i];
	    if (field[dx][dy].id == ID_DIGGER)
	      {
		if (!digger_death) DieDigger();
		break;
	      }
	    else
	      if (field[dx][dy].id == ID_NOTHING)
		{
		  larrow = ghosts[g].arrow;
		  ghosts[g].x = dx;
		  ghosts[g].y = dy;
		  if (dx < x) ghosts[g].arrow = AGHOSTL; else
		    if (dx > x) ghosts[g].arrow = AGHOSTR; else
		      if (dy < y) ghosts[g].arrow = AGHOSTU; else
			if (dy > y) ghosts[g].arrow = AGHOSTD;
		  arrow = ghosts[g].arrow;
		  if (ghosts[g].type == TGHOST90LR)
		    switch (larrow)
		      {
		      case AGHOSTL:
			if (arrow == AGHOSTD) ghosts[g].lastturn = LGHOSTL;
			else
			  if (arrow == AGHOSTU) ghosts[g].lastturn = LGHOSTR;
			break;
		      case AGHOSTR:
			if (arrow == AGHOSTD) ghosts[g].lastturn = LGHOSTR;
			else
			  if (arrow == AGHOSTU) ghosts[g].lastturn = LGHOSTL;
			break;
		      case AGHOSTU:
			if (arrow == AGHOSTL) ghosts[g].lastturn = LGHOSTL;
			else
			  if (arrow == AGHOSTR) ghosts[g].lastturn = LGHOSTR;
			break;
		      case AGHOSTD:
			if (arrow == AGHOSTL) ghosts[g].lastturn = LGHOSTR;
			else
			  if (arrow == AGHOSTR) ghosts[g].lastturn = LGHOSTL;
			break;
		      }
		  CopyField(&field[dx][dy], &field[x][y]);
		  switch (arrow)
		    {
		    case AGHOSTL:
		      field[dx][dy].flags = 
			(field[dx][dy].flags & !F_GHOST_MOVEMASK) | 
			F_GHOST_LEFT;
		      break;
		    case AGHOSTR:
		      field[dx][dy].flags = 
			(field[dx][dy].flags & !F_GHOST_MOVEMASK) | 
			F_GHOST_RIGHT;
		      break;
		    case AGHOSTU:
		      field[dx][dy].flags = 
			(field[dx][dy].flags & !F_GHOST_MOVEMASK) | 
			F_GHOST_UP;
		      break;
		    case AGHOSTD:
		      field[dx][dy].flags = 
			(field[dx][dy].flags & !F_GHOST_MOVEMASK) | 
			F_GHOST_DOWN;
		      break;
		    }
		  field[dx][dy].pixmap_id = 
		    id2pixmap_id(field[dx][dy].id, field[dx][dy].flags);
		  field[dx][dy].todraw = True;
		  FillFieldNothing(&field[x][y]);
		  field[x][y].todraw = True;
		  break;
		}
	  }
    }
} /* GhostStep() */

void TimeStep()
{
  if (lefttime != 0)
    {
      lefttime--;
      Restore_LeftTime();
    }
  if ((lefttime == 0) && (!digger_death)) DieDigger();
} /* TimeStep() */

char rec_data[2500];
int rec_data_pointer;

void RecordStep()
{
  char arrow;

  if (lefttime == 0) return;

  if (kb_left) arrow = ADIGGERL;
  else if (kb_right) arrow = ADIGGERR;
  else if (kb_up) arrow = ADIGGERU;
  else if (kb_down) arrow = ADIGGERD;
  else arrow = ADIGGERN;

  rec_data[rec_data_pointer] = arrow;
  /*printf("%d ", rec_data_pointer);*/
  rec_data_pointer++;
}

void ReplayStep()
{
  char s[2];

  if (lefttime == 0) return;

  if (rec_data_pointer % 8 >= 4) strcpy(s, "R"); else strcpy(s, " ");
  WriteTextStr(s, 39, vertvar, kcf_weiss, kcb_rot);

  kb_left = False; kb_right = False; kb_up = False; kb_down = False;

  switch (rec_data[rec_data_pointer])
    {
    case ADIGGERL:
      kb_left = True;
      break;
    case ADIGGERR:
      kb_right = True;
      break;
    case ADIGGERU:
      kb_up = True;
      break;
    case ADIGGERD:
      kb_down = True;
      break;
    }
  rec_data_pointer++;
}

void Ton_Bearbeitung_1()
{
  ton_schritt = False;
  ton_steine = False;
  ton_diamant = False;
} /* Ton_Bearbeitung_1() */

void Ton_Bearbeitung_2()
{
  if (ton_diamant) sound(TON_DIAMANT);
  else if (ton_steine) sound(TON_STEINE);
  else if (ton_schritt) sound(TON_SCHRITT);

  ton_schritt = False;
  ton_steine = False;
  ton_diamant = False;
} /* Ton_Bearbeitung_2() */

void NextStep()
{
  Ton_Bearbeitung_1();

  if (replay) ReplayStep(); else RecordStep();
  DiggerStep();
  StoneDiamondStep();
  GhostStep();
  TimeStep();

  Ton_Bearbeitung_2();
} /* NextStep() */

Bool rl_left, rl_right, rl_up, rl_down;
Bool rd_left, rd_right, rd_up, rd_down;

void ClearKeys()
{ 
  kb_left = False; kb_right = False; kb_up = False; kb_down = False;
  rl_left = False; rl_right = False; rl_up = False; rl_down = False;
  rd_left = False; rd_right = False; rd_up = False; rd_down = False;
}

int takt_zaehler_one_step, takt_zaehler_diamond_blink;

void ResetZaehler()
{
  takt_zaehler_one_step = -1; takt_zaehler_diamond_blink = -1;
  if (!replay) bzero(rec_data, sizeof(rec_data));
  rec_data_pointer = 0;
}

Bool DoTakt()
{
  takt = False;
  intakt = True;
  setitimer(ITIMER_REAL, &timervalue, 0);
  if (++takt_zaehler_one_step >= STEP_MAX) takt_zaehler_one_step = 0;
  if (++takt_zaehler_diamond_blink >= BLINK_MAX)
    takt_zaehler_diamond_blink = 0;
  
  if (takt_zaehler_one_step == 0)
  {
    if (new_level_number)
    {
      if (LoadLevel(new_level_number, False))
      {
        ChangePS();
        Init_First_Line();
        DrawTextField();
        DrawField();
        ClearKeys();
        ResetZaehler();
        XFlush(display);
        intakt = False;
        return(True);
      }
      else
      {
        intakt = False;
        return(False);
      }
    }
    else
    {
      if (wait_count) wait_count--;
      if (!wait_count)
      {
        if ((!inpause) || (step)) {NextStep(); step = False;}
      }
      if (levelready)
      { 
        if (!last_level)
          new_level_number = akt_level_number + 1;
        else
          alllevelsready = True;
      }
    }
    if (rl_left)  {kb_left  = False; rl_left  = False;}
    if (rl_right) {kb_right = False; rl_right = False;}
    if (rl_up)    {kb_up    = False; rl_up    = False;}
    if (rl_down)  {kb_down  = False; rl_down  = False;}
    rd_left  = True;
    rd_right = True;
    rd_up    = True;
    rd_down  = True;
  }
  
  if (takt_zaehler_one_step == STEP_HALF)
  {
    if ((!wait_count) && (!inpause))
    {
      if (STEP_HALF > 0) DiggerHalfStep();
      TimeStep();
    }
  }

  if ((takt_zaehler_diamond_blink == 0) && (blink))
  {
    DiamondBlinkStep();
  }

  if ((takt_zaehler_one_step == 0) || (takt_zaehler_one_step == STEP_HALF) ||
      (takt_zaehler_diamond_blink == 0))
  {
    DrawTextField();
    DrawField();
    XFlush(display);
  }
  
  intakt = False;
  return(True);
} /* Bool DoTakt() */

void RunLevels(int von_level, int bis_level)
{
  XSetWindowAttributes windowattributes;
  unsigned long valuemask;
  Bool done;
  XEvent event;
  KeySym keysym;
  char scheat[4] = "";

  in_runlevels = True;
  windowattributes.background_pixel = BlackPixel(display, screen);
  valuemask = CWBackPixel;
  XChangeWindowAttributes(display, mainwindow, valuemask, &windowattributes);

  ClearCharField(BlackPixel(display, screen), False);
  ResetZaehler();
  alllevelsready = False;
  lives = 20;
  score = 0;
  new_level_number = von_level;
  replay = False;
  takt = True;

  done = False;
  back = False;
  while (!done)
    {
      if (!XEventsQueued(display, QueuedAfterReading))
 	{
          if (takt) 
          {
            if (DoTakt())
            {
              if ((new_level_number) && (bis_level != -1) &&
                  (new_level_number > bis_level)) done = True;
              if (alllevelsready) done = True;
            }
            else
            {
              quit = True;
              done = True;
            }
          }
          XDigger_Idle();
	} /* if (!XEventsQueued()) */
      else
	{
	  XNextEvent(display, &event);
          if (event.xany.window == iconwindow)
            Handle_IconWindow_Event(&event); else
	  switch(event.type)
	    {
	    case ButtonPress:
	      if (event.xbutton.button == Button3)
		{
		  if (lives > 1)
		    {
		      if (!cheat) lives--;
		      new_level_number = akt_level_number;
		    }
		  else done = True;
		}
	      if ((event.xbutton.button == Button2) && cheat)
		{
		  if (!last_level)
		    new_level_number = akt_level_number + 1;
		  else
		    done = True;
		}
	      break;
	    case Expose:
	      Handle_ExposeEvent(&event.xexpose);
	      break;
	    case MappingNotify:
	      XRefreshKeyboardMapping(&event.xmapping);
	      break;
	    case KeyPress:
	      keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
	      switch (keysym)
              {
		case XK_Return:
		  wait_count = 0;
		  break;
		case XK_Left:
		  kb_left = True; rd_left = False;
		  if (cheat && inpause) step = True;
		  break;
	        case XK_Right:
		  kb_right = True; rd_right = False;
		  if (cheat && inpause) step = True;
		  break;
		case XK_Up:
		  kb_up = True; rd_up = False;
		  if (cheat && inpause) step = True;
		  break;
		case XK_Down:
		  kb_down = True; rd_down = False;
		  if (cheat && inpause) step = True;
		  break;
		case XK_b:
		  if (event.xkey.state & ControlMask)
                  {
                    if (lives > 1)
                    {
                      if (!cheat) lives--;
                      new_level_number = akt_level_number;
                    }
                    else done = True;
                  }
		  break;
		case XK_Escape:
		  replay = False;
                  if (lives > 1)
                  {
                    if (!cheat) lives--;
                    new_level_number = akt_level_number;
                  } else done = True;
		  break;
		case XK_Insert:
		  if (cheat) digger_candie = False;
		  break;
		case XK_Delete: 
		  if (cheat) digger_candie = True;
		  break;
		  case XK_Home:
		    if (cheat)
		      {
			if (event.xkey.state & ShiftMask)
			  {if (akt_level_number > 1)
			    new_level_number = akt_level_number - 1;}
			else
			  {if (!last_level)
			    new_level_number = akt_level_number + 1;}
		      }
		    break;
		case XK_p:
		  if (cheat) inpause = !inpause;
		  break;
		case XK_q: 
		  quit = True;
		  done = True;
		  break;
		case XK_r:
		  new_level_number = akt_level_number;
		  replay = True;
		  break;
		case XK_x:
                case XK_m:
                  back = True;
		  done = True;
		  break;
		case XK_space:
		  if (cheat) step = True;
		  break;
                case XK_plus:
                case XK_KP_Add:
                  XResizeWindow(display, mainwindow, 320*(xpmmag+1),
                                (240 + vertvar*16)*(xpmmag+1));
                  break;
                case XK_minus:
                case XK_KP_Subtract:
                  if (xpmmag > 1)
                    XResizeWindow(display, mainwindow, 320*(xpmmag-1),
                                  (240 + vertvar*16)*(xpmmag-1));
                  break;
		}
	      if ((keysym == XK_9) || (keysym == XK_d))
		{
		  if (keysym == XK_9) strcat(scheat, "9");
		  if (keysym == XK_d) strcat(scheat, "d");
		  if (strcmp(scheat, "99d") == 0)
		    {
		      XBell(display, 0);
		      cheat = (!cheat);
		      ones_cheated = True;
		      ChangePS();
		    }
		  if (strlen(scheat) == 3) strcpy(scheat, "");
		}
	      else
		strcpy(scheat, "");
	      break;
	    case KeyRelease:
	      keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);

	      if (keysym == XK_Left) {
		if (rd_left) kb_left = False;
		else rl_left = True;
              }
	      if (keysym == XK_Right) {
		if (rd_right) kb_right = False;
		else rl_right = True;
              }
	      if (keysym == XK_Up) {
		if (rd_up) kb_up = False;
		else rl_up = True;
              }
	      if (keysym == XK_Down) {
		if (rd_down) kb_down = False;
		else rl_down  = True;
              }
	      break;
	    case ClientMessage:
	      if (event.xclient.data.l[0] == protocol[0])
		{
		  quit = True;
		  done = True;
		  break;
		}
	      break ;
	    } /* switch(event.type) */

	} /* if (!XEventsQueued()) else */
    } /* while (!done) */
  in_runlevels = False;
  if (takt) setitimer(ITIMER_REAL, &timervalue, 0);
  strcpy(LastArgv, "");
} /* RunLevels() */

/*---------------------------------------------------------------------------*/

void ALookAtTheRoomsTakt()
{
  takt = False;
  intakt = True;
  setitimer(ITIMER_REAL, &timervalue, 0);
  if (++takt_zaehler_diamond_blink == 3) takt_zaehler_diamond_blink = 0;

  if ((takt_zaehler_diamond_blink == 1) && (blink))
  {
    DiamondBlinkStep();
  }

  if (takt_zaehler_diamond_blink == 1)
  {
    DrawTextField();
    DrawField();
    XFlush(display);
  }
  
  intakt = False;
} /* DoTakt() */

void ALookAtTheRooms()
{
  XSetWindowAttributes windowattributes;
  unsigned long valuemask;
  XEvent event;
  KeySym keysym;
  Bool done = False;

  in_runlevels = True;
  strcpy(LastArgv, " (Look)");
  windowattributes.background_pixel = BlackPixel(display, screen);
  valuemask = CWBackPixel;
  XChangeWindowAttributes(display, mainwindow, valuemask, &windowattributes);

  ClearCharField(BlackPixel(display, screen), False);
  lives = 20;
  score = 0;
  new_level_number = 1;

  while (!done)
    {
      if (new_level_number)
	{
	  if (LoadLevel(new_level_number, False))
          {
            Init_First_Line();
            DrawTextField();
            DrawField();
            ClearKeys();
          }
          else
          {
            quit = True;
            done = True;
          }
	}
      if (!XEventsQueued(display, QueuedAfterReading))
	{
	  if (takt)
	    {
	      ALookAtTheRoomsTakt();
	    }
	  XDigger_Idle();
	  XFlush(display);
	} /* if (!XEventsQueued()) */
      else
	{
	  XNextEvent(display, &event);
          if (event.xany.window == iconwindow)
            Handle_IconWindow_Event(&event); else
	  switch(event.type)
	    {
	    case Expose:
	      Handle_ExposeEvent(&event.xexpose);
	      break;
	    case MappingNotify:
	      XRefreshKeyboardMapping(&event.xmapping);
	      break;
	    case ButtonPress:
	      if (event.xbutton.button == Button1)
		{
		  if (!last_level)
		    new_level_number = akt_level_number + 1;
		  else
		    done = True;
		}
	      if (event.xbutton.button == Button3)
		{
		  if (akt_level_number > 1)
		    new_level_number = akt_level_number - 1;
		  else
		    done = True;
		}
	      break;
	    case KeyPress:
	      keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
	      switch (keysym)
              {
                case XK_Left:
                case XK_Down:
		  if (akt_level_number > 1)
		    new_level_number = akt_level_number - 1;
                  break;
                case XK_Right:
                case XK_Up:
		  if (!last_level)
		    new_level_number = akt_level_number + 1;
                  break;
                case XK_Return:
		  if (!last_level)
		    new_level_number = akt_level_number + 1;
		  else
		    done = True;
		  break;
		case XK_q: 
		  quit = True;
		  done = True;
		  break;
		case XK_x: 
                case XK_m:
                case XK_Escape:
                  done = True;
		  break;
                case XK_plus:
                case XK_KP_Add:
                  XResizeWindow(display, mainwindow, 320*(xpmmag+1),
                                (240 + vertvar*16)*(xpmmag+1));
                  break;
                case XK_minus:
                case XK_KP_Subtract:
                  if (xpmmag > 1)
                    XResizeWindow(display, mainwindow, 320*(xpmmag-1),
                                  (240 + vertvar*16)*(xpmmag-1));
                  break;
              }
	      break;
	    case ClientMessage:
	      if (event.xclient.data.l[0] == protocol[0])
		{
		  /*exit(0);*/
		  quit = True;
		  done = True;
		  break;
		}
	    } /* switch(event.type) */
	} /* if (!XEventsQueued()) else */
    } /* while (!done) */
  strcpy(LastArgv, "");
  in_runlevels = False;
  if (takt) setitimer(ITIMER_REAL, &timervalue, 0);
} /* ALookAtTheRooms() */

/*---------------------------------------------------------------------------*/

void Edit_SaveLevel(int level_number)
{
/*   char filename[256]; */
  unsigned char file_daten[0x9c], *c, b;
  int x, y, id, flags, i, d;
  FILE *filehandle;
  
  void Save_Ghost()
  {
    switch (flags & F_GHOST_TYPEMASK)
    {
    case F_GHOST_TYPE180:
      id = 3;
      break;
    case F_GHOST_TYPEL:
      id = 7;
      break;
    case F_GHOST_TYPER:
      id = 15;
      break;
    case F_GHOST_TYPELR:
      id = 11;
      break;
    }
    switch (flags & F_GHOST_MOVEMASK)
    {
    case F_GHOST_DOWN:
      d = 0;
      break;
    case F_GHOST_UP:
      d = 1;
      break;
    case F_GHOST_RIGHT:
      d = 2;
      break;
    case F_GHOST_LEFT:
      d = 3;
      break;
    }
    if (number_ghosts < MAX_GHOSTS)
    {
      if (number_ghosts & 1)
        file_daten[0x94 + number_ghosts/2] =
          (file_daten[0x94 + number_ghosts/2] & 0xf0) | (d);
      else
        file_daten[0x94 + number_ghosts/2] =
          (file_daten[0x94 + number_ghosts/2] & 0x0f) | (d << 4);
      number_ghosts++;
    }
  }
  
  bzero(file_daten, sizeof(file_daten));
  number_ghosts = 0;
  c = file_daten;
  for (y=0; y<14; y++) for (x=0; x<10; x++, c++)
    {
        id = field[x*2][y].id;
        flags = field[x*2][y].flags;
        if (id == ID_DIGGER)
        {
            diggerposx = x*2;
            diggerposy = y;
        }
        if (id == ID_GHOST)
          Save_Ghost();
        *c = (*c & 0x0f) | (id << 4);

  /* ------------------------------------------------------------------ */

        id = field[x*2+1][y].id;
        flags = field[x*2+1][y].flags;
        if (id == ID_DIGGER)
        {
            diggerposx = x*2+1;
            diggerposy = y;
        }
        if (id == ID_GHOST)
          Save_Ghost();
        *c = (*c & 0xf0) | id;
    }
  c += 3; 
  if (!last_level) *c = 1;
  c += 2;
  *(c++) = diggerposx;
  *(c++) = diggerposy + 2;
  b = number_diamonds / 10 * 0x10 + number_diamonds % 10;
  *(c++) = b;

  if ((filehandle = fopen(level_filename, "r+")) == NULL)
  {
    XBell(display, -50);
    fprintf(stderr, "%s: can't open %s for rw\n", progname, level_filename);
    return;
  }
  i = fseek(filehandle, (level_number - 1) * 0x9c, SEEK_SET);
  i = fwrite(file_daten, 0x9c, 1, filehandle);
  fclose(filehandle);
  if (i==1)
    printf("%s: Level saved.\n", progname);
} /* edit_savelevel(int level_number) */

struct ttools {unsigned char id, flags;}
tools[] = {
  {ID_NOTHING, 0}, {ID_GROUND, 0}, {ID_WALL, 0}, {ID_STONE, 0},
  {ID_DIAMOND, 0}, {ID_CHANGER, 0}, {ID_EXIT, 0}, {ID_DIGGER, F_DIGGER_DOWN0},
  {ID_GHOST, F_GHOST_DOWN + F_GHOST_TYPE180},
  {ID_GHOST, F_GHOST_UP + F_GHOST_TYPE180},
  {ID_GHOST, F_GHOST_RIGHT + F_GHOST_TYPE180},
  {ID_GHOST, F_GHOST_LEFT + F_GHOST_TYPE180},
  {ID_GHOST, F_GHOST_DOWN + F_GHOST_TYPEL},
  {ID_GHOST, F_GHOST_UP + F_GHOST_TYPEL},
  {ID_GHOST, F_GHOST_RIGHT + F_GHOST_TYPEL},
  {ID_GHOST, F_GHOST_LEFT + F_GHOST_TYPEL},
  {ID_GHOST, F_GHOST_DOWN + F_GHOST_TYPER},
  {ID_GHOST, F_GHOST_UP + F_GHOST_TYPER},
  {ID_GHOST, F_GHOST_RIGHT + F_GHOST_TYPER},
  {ID_GHOST, F_GHOST_LEFT + F_GHOST_TYPER},
  {ID_GHOST, F_GHOST_DOWN + F_GHOST_TYPELR},
  {ID_GHOST, F_GHOST_UP + F_GHOST_TYPELR},
  {ID_GHOST, F_GHOST_RIGHT + F_GHOST_TYPELR},
  {ID_GHOST, F_GHOST_LEFT + F_GHOST_TYPELR}
};

int auswahl_pos[3] = {1, 2, 0},
  auswahl_id[3] = {ID_GROUND, ID_WALL, ID_NOTHING},
  auswahl_flags[3] = {0,0,0},
    last_pressed_button = 0;

void Handle_Tools_ExposeEvent(XExposeEvent *xexpose)
{
  int i;
  
  for (i=0; i<24; i++)
  {
    CopyOnePixmap_Window(toolswindow,
                         id2pixmap_id(tools[i].id, tools[i].flags), i%4, i/4);
  }

  for (i=0; i<3; i++)
  {
    XDrawRectangle(display, toolswindow, gc,
                   (auswahl_pos[i]%4)*16*xpmmag, (auswahl_pos[i]/4)*16*xpmmag,
                   16*xpmmag-1, 16*xpmmag-1);
    XFillRectangle(display, toolswindow, gc,
                   (auswahl_pos[i]%4)*16*xpmmag + (i*5+1)*xpmmag,
                   (auswahl_pos[i]/4)*16*xpmmag,
                   4*xpmmag, 4*xpmmag);
  }
  XFlush(display);
}

int Anzahl_Ghosts()
{
  int erg = 0, x, y;

  for (y=0; y<14; y++) for (x=0; x<20; x++)
    if (field[x][y].id == ID_GHOST) erg++;
  return erg;
}

void Handle_ButtonPressEvent_mainwindow(XButtonEvent *xbutton)
{
  int i=0, x, y;

  if (xbutton->button == Button1) i=0;
  if (xbutton->button == Button2) i=1;
  if (xbutton->button == Button3) i=2;
  last_pressed_button = i;

  x = xbutton->x / (16*xpmmag);
  y = xbutton->y / (16*xpmmag) - (vertvar+1);

  if (y < 0)
  {
    if (xbutton->button == Button1)
      {
        if (number_diamonds > 0) number_diamonds--;
      } else
      {        
        if (number_diamonds < 99) number_diamonds++;
      }
    Restore_Diamonds();
    DrawTextField();
  }
  else
  {
    if ((auswahl_id[i] == ID_GHOST) && (Anzahl_Ghosts() >= MAX_GHOSTS))
    {
      fprintf(stderr, "%s: You can´t have more than 16 ghosts.\n", progname);
    }
    else
    {
      FillFieldNothing(&field[x][y]);
      field[x][y].id = auswahl_id[i];
      field[x][y].flags = auswahl_flags[i];
      field[x][y].pixmap_id = id2pixmap_id(field[x][y].id, field[x][y].flags);
      field[x][y].todraw = True;
      DrawField();
    }
  }
  XFlush(display);
}

void Handle_MotionEvent_mainwindow(XMotionEvent *xmotion)
{
  int x, y;

  if (xmotion->state == 0) return;
  
  x = xmotion->x / (16*xpmmag);
  y = xmotion->y / (16*xpmmag) - (vertvar+1);

  if (y < 0)
  {
  }
  else
  {
    if ((auswahl_id[last_pressed_button] == ID_GHOST) &&
        (Anzahl_Ghosts() >= MAX_GHOSTS))
    {
      fprintf(stderr, "%s: You can´t have more than 16 ghosts.\n", progname);
    }
    else
    {
      FillFieldNothing(&field[x][y]);
      field[x][y].id = auswahl_id[last_pressed_button];
      field[x][y].flags = auswahl_flags[last_pressed_button];
      field[x][y].pixmap_id = id2pixmap_id(field[x][y].id, field[x][y].flags);
      field[x][y].todraw = True;
      DrawField();
    }
  }
  XFlush(display);
}

void Handle_ButtonPressEvent_toolswindow(XButtonEvent *xbutton)
{
  int i=0, x, y;

  if (xbutton->button == Button1) i=0;
  if (xbutton->button == Button2) i=1;
  if (xbutton->button == Button3) i=2;
  
  x = xbutton->x / (16*xpmmag);
  y = xbutton->y / (16*xpmmag);
  auswahl_pos[i] = y * 4 + x;
  auswahl_id[i] = tools[auswahl_pos[i]].id;
  auswahl_flags[i] = tools[auswahl_pos[i]].flags;

  Handle_Tools_ExposeEvent(NULL);
}

void LevelEditor()
{
  int win_width, win_height;
  XSizeHints sizehints;
  XSetWindowAttributes windowattributes;
  unsigned long valuemask;
  XEvent event;
  KeySym keysym;
  Bool done = False;
  
  in_runlevels = True;
  in_LevelEditor = True;
  strcpy(LastArgv, " (Edit)");
  windowattributes.background_pixel = BlackPixel(display, screen);
  valuemask = CWBackPixel;
  XChangeWindowAttributes(display, mainwindow, valuemask, &windowattributes);

  ClearCharField(BlackPixel(display, screen), False);
  if (pixmaps[PXID_GHOST_DOWN_180] == 0) Make16x16Pixmaps_Editor();
  lives = 20;
  score = 0;
  new_level_number = 1;

  win_width = 4*16 * xpmmag;
  win_height = 6*16 * xpmmag;
  toolswindow = XCreateSimpleWindow(display, rootwindow, 0, 0,
                                     win_width, win_height, border_width,
                                     WhitePixel(display, screen),
                                     BlackPixel(display, screen));
  sizehints.width = win_width;
  sizehints.height = win_height;
  sizehints.min_width = win_width;
  sizehints.min_height = win_height;
  sizehints.max_width = win_width;
  sizehints.max_height = win_height;
  sizehints.flags = PSize | PMinSize | PMaxSize;
  XSetStandardProperties(display, toolswindow, "Tools", "Tools", 
			 None, NULL, 0, &sizehints);
  XSelectInput(display, toolswindow, ButtonPressMask | ButtonMotionMask |
               KeyPressMask | KeyReleaseMask | ExposureMask);
  XMapRaised(display, toolswindow);
/*   XMapSubwindows(display, mainwindow); */
  
  while (!done)
  {
    if (new_level_number)
    {
      if (LoadLevel(new_level_number, True))
      {
        Init_First_Line();
        DrawTextField();
        DrawField();
        XFlush(display);
        ClearKeys();
      }
      else
      {
        done = True;
      }
    }
    if (!XEventsQueued(display, QueuedAfterReading))
    {
      XDigger_Idle();
      XFlush(display);
    }
    else
    {
      XNextEvent(display, &event);
      if (event.xany.window == iconwindow)
        Handle_IconWindow_Event(&event); else
      if (event.xany.window == mainwindow)
        {
          switch(event.type)
          {
            case Expose:
	      Handle_ExposeEvent(&event.xexpose);
              XFlush(display);
              break;
            case MappingNotify:
              XRefreshKeyboardMapping(&event.xmapping);
              break;
            case ButtonPress:
              Handle_ButtonPressEvent_mainwindow(&event.xbutton);
              break;
            case MotionNotify:
              Handle_MotionEvent_mainwindow(&event.xmotion);
              break;
            case KeyPress:
              keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
              switch (keysym)
              {
                case XK_p:
                  Edit_SaveLevel(akt_level_number);
                  in_LevelEditor = False;
                  RunLevels(akt_level_number, akt_level_number);
                  new_level_number = akt_level_number;
                  in_runlevels = True;
                  in_LevelEditor = True;
                  strcpy(LastArgv, " (Edit)");
                  done = quit;
                  break;
                case XK_q:
                  quit = True;
                  done = True;
                  break;
                case XK_s:
                  Edit_SaveLevel(akt_level_number);
                  break;
		case XK_x:
                case XK_m:
		  done = True;
		  break;
                case XK_Home:
                  if (event.xkey.state & ShiftMask)
                  {if (akt_level_number > 1)
                     new_level_number = akt_level_number - 1;}
                  else
                  {if (!last_level)
                     new_level_number = akt_level_number + 1;}
                  break;
                case XK_KP_Add:
                  XResizeWindow(display, mainwindow, 320*(xpmmag+1),
                                (240 + vertvar*16)*(xpmmag+1));
                  break;
                case XK_minus:
                case XK_KP_Subtract:
                  if (xpmmag > 1)
                    XResizeWindow(display, mainwindow, 320*(xpmmag-1),
                                  (240 + vertvar*16)*(xpmmag-1));
                  break;
              }
              break;
            case ClientMessage:
              if (event.xclient.data.l[0] == protocol[0])
              {
                /*exit(0);*/
		quit = True;
		done = True;
		break;
              }
          } /* switch(event.type) */
        }
        else
        {
          switch(event.type)
          {
            case Expose:
	      Handle_Tools_ExposeEvent(&event.xexpose);
              XFlush(display);
              break;
            case MappingNotify:
              XRefreshKeyboardMapping(&event.xmapping);
              break;
            case ButtonPress:
              Handle_ButtonPressEvent_toolswindow(&event.xbutton);
              break;
            case KeyPress:
              keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
              switch (keysym)
              {
                case XK_q: 
                  quit = True;
                  done = True;
                  break;
              }
              break;
            case ClientMessage:
              if (event.xclient.data.l[0] == protocol[0])
              {
                /*exit(0);*/
		quit = True;
		done = True;
		break;
              }
          } /* switch(event.type) */
        }
    } /* if (!XEventsQueued()) else */
  } /* while (!done) */
  
  XDestroyWindow(display, toolswindow);  
  strcpy(LastArgv, "");
  in_runlevels = False;
  in_LevelEditor = False;
}
