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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "configure.h"
#include "drawpixmaps.h"
#include "drawtext.h"
#include "icon.h"
#include "runlevels.h"
#include "xdigger.h"

#define NAMELENGH 20

typedef struct
{
  int score;
  char name[NAMELENGH + 1];
} ScoreEntry;

ScoreEntry highscore[20];

char* get_highscore_dir()
{
  static char highscore_dir[1024] = {0};

  char* stat_home = getenv("XDG_STATE_HOME");
  if (stat_home)
  {
    int ret = snprintf(highscore_dir, sizeof(highscore_dir), "%s/xdigger", stat_home);
    if (ret < 0) {
      return NULL;
    } else if (ret >= sizeof(highscore_dir)) {
      return NULL;
    }
    return highscore_dir;
  }
  else
  {
    char* home = getenv("HOME");
    if (!home) {
      return NULL;
    }
    else
    {
      int ret = snprintf(highscore_dir, sizeof(highscore_dir), "%s/.local/state/xdigger", home);
      if (ret < 0) {
        return NULL;
      } else if (ret >= sizeof(highscore_dir)) {
        return NULL;
      }
      return highscore_dir;
    }
  }
}

void LoadHighScore()
{
  char filename[256];
  FILE *filehandle;
  int i, n = 0;

  for (i=0; i<NAMELENGH; i++)
    {
      highscore[i].score = 0;
      strcpy(highscore[i].name, "");
    }

  char* highscore_dir = get_highscore_dir();
  if (highscore_dir) {
    snprintf(filename, sizeof(filename), "%s/xdigger.hiscore",
             highscore_dir);

    if ((filehandle = fopen(filename, "r")) == NULL)
    {
      fprintf(stderr, "%s: can't read %s\n", progname, filename);
    }
  }

  if (filehandle != NULL)
    {
      n = fread(highscore, sizeof(highscore), 1, filehandle);
      fclose(filehandle);
    }

  if (n < 1) /*sizeof(highscore)*/
    {
      highscore[0].score = 10000; strcpy(highscore[0].name, "--------------");
      highscore[1].score =  9000; strcpy(highscore[1].name, "XDigger");
      highscore[2].score =  8000; strcpy(highscore[2].name, "(c) 1988-99 by");
      highscore[3].score =  7000; strcpy(highscore[3].name, "Alexander Lang");
      highscore[4].score =  6000; strcpy(highscore[4].name, "--------------");
    }
} /* LoadHighScore() */

void SaveHighScore()
{
  char filename[256];
  FILE *filehandle;
  int n = 0;

  char* highscore_dir = get_highscore_dir();
  if (highscore_dir)
  {
    // create highscore directory if it does not already exist
    struct stat st;
    if (!(stat(highscore_dir, &st) == 0 && S_ISDIR(st.st_mode))) {
      errno = 0;
      int status = mkdir(highscore_dir, 0700);
      if (status < 0) {
        fprintf(stderr, "failed to create highscore directory: %s: %s\n", highscore_dir, strerror(errno));
      }
    }

    snprintf(filename, sizeof(filename), "%s/xdigger.hiscore",
             highscore_dir);
    if ((filehandle = fopen(filename, "w")) == NULL)
    {
      XBell(display, -50);
      fprintf(stderr, "%s: can't write %s\n", progname, filename);
    }
    if (filehandle != NULL)
    {
      n = fwrite(highscore, sizeof(highscore), 1, filehandle);
      fclose(filehandle);
    }
  }

  if (n < 1) /*sizeof(highscore)*/
    fprintf(stderr, "%s: an error occured while writing highscorefile\n",
            progname);
} /* SaveHighScore() */

/* char *GetUserName() */
/* { */
/*   struct passwd *who; */
/*   char name[257], *c; */

/*   who = getpwuid(getuid()); */
/*   strncpy(name, who->pw_gecos, 256); */
/*   c = strchr(name, ',') ; */
/*   if (c != NULL) *c = '\0'; */
/*   return(name); */
/* } char *GetUserName() */

void GetUserName(char *dest, size_t n)
{
  struct passwd *who;
  char name[257], *c;

  who = getpwuid(getuid());
  snprintf(name, sizeof(name), "%s", who->pw_gecos);
  c = strchr(name, ',') ;
  if (c != NULL) *c = '\0';
  snprintf(dest, n, "%s", name);
} /* GetUserName(char *dest, size_t n) */

int InsertScore(int score, char *name)
{
  int i, j, erg;

  erg = -1;
  if (!ones_cheated) for (i=0; i<20; i++)
    if (score > highscore[i].score)
    {
      erg = i;
      for (j=19; j>i; j--)
	{
	  highscore[j].score = highscore[j-1].score;
	  strncpy(highscore[j].name, highscore[j-1].name, sizeof(highscore[j].name));
	}
      highscore[i].score = score;
      strncpy(highscore[i].name, name, sizeof(highscore[i].name));
      break;
    }
  return(erg);
} /* int InsertScore(int score, char *name) */

void InitHighScoreText()
{
  int i;
  char entry[NAMELENGH + 10];

  WriteTextStr("...well done, please enter your name :", 1, 2, 
	       kcf_gelb, kcb_tuerkis);
  WriteTextStr("HIGHSCORE :", 8, 4, kcf_gelb, kcb_tuerkis);
  WriteTextStr("\217\217\217\217\217\217\217\217\217\217\217", 8, 5, 
	       kcf_gelb, kcb_tuerkis);

  for (i=0; i<20; i++)
    {
      snprintf(entry, sizeof(entry), "%.6d  %s", highscore[i].score, highscore[i].name);
      WriteTextStr(entry, 10, 7+i, kcf_gelb, kcb_tuerkis);
    }
} /* InitHighScoreText() */

void HighScore(int score)
{
  XSetWindowAttributes setwindowattributes;
  XWindowAttributes windowattributes;
  unsigned long valuemask;
  XEvent event;
  Bool Done = False;
  KeySym keysym;
  char buffer[20];
  int bufsize = 20, count;
  char name[NAMELENGH + 1], *nameinput;
  int inpy, y;
/*   XComposeStatus cs; */

  strcpy(LastArgv, " (HiScore)");
  setwindowattributes.background_pixel = kcb_tuerkis;
  valuemask = CWBackPixel;
  XChangeWindowAttributes(display, mainwindow, valuemask, &setwindowattributes);

  LoadHighScore();
  GetUserName(name, 20);
  y = InsertScore(score, name);
  nameinput = highscore[y].name;
  inpy = y + 7;
  ClearCharField(kcb_tuerkis, True);
  InitHighScoreText();
  if (y>=0) 
    WriteTextStr("\177", 18 + strlen(nameinput), inpy, kcf_gelb, kcb_tuerkis);
  DrawTextField();

  while (!Done)
    {
      if (!XEventsQueued(display, QueuedAfterReading))
	{
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
              /* Resize ? */
              XGetWindowAttributes(display, mainwindow, &windowattributes);
              if (xpmmag != windowattributes.width / 320)
                ResizeXPixmaps(windowattributes.width / 320);
              
	      Mark_CharField(&event.xexpose, 30+2*vertvar);
	      if (event.xexpose.count == 0)
		DrawTextField();
	      break;
	    case MappingNotify:
	      XRefreshKeyboardMapping(&event.xmapping);
	      break;
	    case KeyPress:
	      count = XLookupString(&event.xkey, buffer, bufsize, &keysym, 
				    NULL);
	      buffer[count] = '\0';
/* 	      keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0); */
	      if ((strlen(nameinput) < 20) && (strlen(buffer) == 1) &&
		  (0x20 <= buffer[0]) && (y>=0))
		{
		  strncat(nameinput, buffer, NAMELENGH - strlen(nameinput));
		  WriteTextStr(nameinput, 18, inpy, kcf_gelb, kcb_tuerkis);
		  WriteTextStr("\177", 18 + strlen(nameinput), inpy, 
			       kcf_gelb, kcb_tuerkis);
		  DrawTextField();
		}
	      switch (keysym)
		{
		case XK_BackSpace:
		  if ((strlen(nameinput) > 0) && (y>=0))
		    {
		      nameinput[strlen(nameinput) - 1] = '\0';
		      WriteTextStr(nameinput, 18, inpy, kcf_gelb, kcb_tuerkis);
		      WriteTextStr("\177 ", 18 + strlen(nameinput), inpy, 
				   kcf_gelb, kcb_tuerkis);
		      DrawTextField();
		    }
		  break;
		case XK_Return:
		  Done = True;
		  break;
		case XK_Escape:
		  Done = True;
		  break;
		}
	      break;
	    case ClientMessage:
	      if (event.xclient.data.l[0] == protocol[0])
		{
		  /*exit(0);*/
		  quit = True;
		  Done = True;
		  break;
		}
	      break ;
	    } /* switch(event.type) */
	} /* if (!XEventsQueued()) else */
    } /* while (!Done) */
  SaveHighScore();
  strcpy(LastArgv, "");
} /* HighScore(int score) */
