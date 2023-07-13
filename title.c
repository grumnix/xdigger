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
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "drawpixmaps.h"
#include "drawtext.h"
#include "icon.h"
#include "xdigger.h"

void Init_Title()
{
  int i;

/*   for (i=0; i<32; i++) */
/*     WriteTextStr("                                        ",  */
/* 		 0, i, kcf_weiss, kcb_blau); */
  WriteTextStr("\234\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\236", 0, 0, kcf_weiss, kcb_blau);
  WriteTextStr("\237\240\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\242\237", 0, 1, kcf_weiss, kcb_blau);
  WriteTextStr("\237\243\234\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\236\251\237", 0, 2, kcf_weiss, kcb_blau);

  for (i=3; i<27+2*vertvar; i++)
  {
    WriteTextStr("\237\243\237", 0, i, kcf_weiss, kcb_blau);
    WriteTextStr("\237\251\237", 37, i, kcf_weiss, kcb_blau);
  }

  WriteTextStr("\200\201\202\203 \210 \211\212\213\214 \211\212\213\214 \200\201\215\215 \133\215\221\222", 7, 6, kcf_weiss, kcb_blau);
  WriteTextStr("\133 \206\207 \133 \133  \215 \133  \215 \133    \133 \223\224", 7, 7, kcf_weiss, kcb_blau);
  WriteTextStr("\133  \133 \133 \133 \244\216 \133 \244\216 \133\217\220  \133\225\226\227", 7, 8, kcf_weiss, kcb_blau);
  WriteTextStr("\133 \260\261 \133 \133 \316\133 \133 \316\133 \133    \133 \230\231", 7, 9, kcf_weiss, kcb_blau);
  WriteTextStr("\252\253\254\255 \262 \263\264\265\266 \263\264\265\266 \252\253\267\267 \133 \232\233", 7, 10, kcf_weiss, kcb_blau);

  WriteTextStr("WRITTEN BY  ALEXANDER LANG", 7, 13, kcf_weiss, kcb_blau);
/*   WriteTextStr("GRAPHIX BY  MARTIN    GUTH", 7, 15, kcf_weiss, kcb_blau); */
  WriteTextStr("GRAPHIX BY  STEFAN  DAHLKE", 7, 15, kcf_weiss, kcb_blau);
  WriteTextStr("HUMBOLDT-UNIVERSITY     \245\246", 7, 17, kcf_weiss, kcb_blau);
  WriteTextStr("         BERLIN         \247\250", 7, 18, kcf_weiss, kcb_blau);
  WriteTextStr("P: PLAY", 11, 20, kcf_weiss, kcb_blau);
  WriteTextStr("L: A LOOK AT THE ROOMS", 9, 22, kcf_weiss, kcb_blau);
  WriteTextStr("\140 1988", 26, 25, kcf_weiss, kcb_blau);
  WriteTextStr("BERLIN", 26, 26, kcf_weiss, kcb_blau);

  WriteTextStr("\237\243\306\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\310\251\237", 0, 27+2*vertvar, kcf_weiss, kcb_blau);
  WriteTextStr("\237\312\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\313\314\237", 0, 28+2*vertvar, kcf_weiss, kcb_blau);
  WriteTextStr("\306\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\235\310", 0, 29+2*vertvar, kcf_weiss, kcb_blau);
} /* Init_Title() */

/*void ClearExposeEvents()
{
  XEvent event;
  
  while (XCheckWindowEvent(display, mainwindow, ExposureMask, &event));
}*/

char Title()
{
  XSetWindowAttributes setwindowattributes;
  XWindowAttributes windowattributes;
  unsigned long valuemask;
  XEvent event;
  Bool Done = False;
  KeySym keysym;

  strcpy(LastArgv, " (Title)");
  setwindowattributes.background_pixel = kcb_blau;
  valuemask = CWBackPixel;
  XChangeWindowAttributes(display, mainwindow, valuemask, &setwindowattributes);

  ClearCharField(kcb_blau, True);
  Init_Title();
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
              /*fprintf(stderr, "%d %d %d %d \n", event.xexpose.x, event.xexpose.y,
                      event.xexpose.width, event.xexpose.height);
              fflush(stderr);*/
              /* Resize ? */
              XGetWindowAttributes(display, mainwindow, &windowattributes);
              if (xpmmag != windowattributes.width / 320)
                ResizeXPixmaps(windowattributes.width / 320);
	      Mark_CharField(&event.xexpose, 29+2*vertvar);
	      if (event.xexpose.count == 0)
		DrawTextField();
	      break;
              /*case ResizeRequest:*/
            case MappingNotify:
	      XRefreshKeyboardMapping(&event.xmapping);
	      break;
	    case ButtonPress:
	      if (event.xbutton.button == Button1) return('p');
	      break;
	    case KeyPress:
	      keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
	      switch (keysym)
              {
		case XK_e: return('e');
		case XK_l: return('l');
		case XK_p: return('p');
                case XK_q: 
		  quit = True;
		  return('q');
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
                  return('q');                  
		}
	      break;
            /*case NoExpose:
              break;
            default:
              fprintf(stderr, "default: %d\n", event.type);
              fflush(stderr);*/
	    } /* switch(event.type) */
	} /* if (!XEventsQueued()) else */
    } /* while (!Done) */
  strcpy(LastArgv, "");
  return(0);
} /* char Title() */
