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

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "defines.h"
#include "drawpixmaps.h"
#include "runlevels.h"
#include "xdigger.h"

Window iconwindow;
Bool in_handle_icon = False;
Bool icon_indiggeridlesequence = False;
int icon_fdiggersequenceidx, icon_fdiggeridleidx;
int last_icon_pixmap = PXID_ICON_DIGGER;

void MakeIconWindow()
{
  XWMHints wmhints;
  /*XIconSize **size_list_return;
  int count_return;*/
  
  /*if (XGetIconSizes(display, mainwindow, size_list_return, &count_return))
  {
    fprintf(stderr, "count: %d", count_return);
    fflush(stderr);
  }*/

  iconwindow = XCreateSimpleWindow(display, rootwindow,
                                   0, 0, 16*iconmag, 16*iconmag, 1,
				   WhitePixel(display, screen),
				   BlackPixel(display, screen));
  XSelectInput(display, iconwindow, ExposureMask);
  wmhints.icon_window = iconwindow;
  wmhints.flags = IconWindowHint;
  XSetWMHints(display, mainwindow, &wmhints); 
}

unsigned char id2iconpixmap_id(int id, int flags)
{
  switch (id)
  {
    case ID_DIGGER : 
      {
        switch (flags & F_DIGGER_MOVEMASK)
        {
          case F_DIGGER_DOWN0:  return(PXID_ICON_DIGGER);
          case F_DIGGER_FOOT:   return(PXID_ICON_DIGGER_FOOT);
          case F_DIGGER_EYE1:   return(PXID_ICON_DIGGER_EYE1);
          case F_DIGGER_EYE2:   return(PXID_ICON_DIGGER_EYE2);
        } 
      }
  }
  return(PXID_ICON_NOTHING);
} /* unsigned char id2pixmap_id(int id, int flags) */

void Handle_Icon()
{
  FieldEntry icon_field;
  /*XWMHints wmhints;*/

  if (!in_xdigger_idle)
    return;

  in_handle_icon = True;
  if ((!icon_indiggeridlesequence) && (MyRand(10) == 1))
    {
      icon_fdiggeridleidx = MyRand(2);
      icon_fdiggersequenceidx = 0;
      icon_indiggeridlesequence = True;
    }
  if (icon_indiggeridlesequence)
    {
      if (++icon_fdiggersequenceidx >
	  fdiggeridlesequence[icon_fdiggeridleidx][0])
	icon_indiggeridlesequence = False;
    }

  icon_field.id = ID_DIGGER;

  if (icon_indiggeridlesequence)
    icon_field.flags =
      (icon_field.flags & !F_DIGGER_MOVEMASK) |
      fdiggeridlesequence[icon_fdiggeridleidx][icon_fdiggersequenceidx];
  else
    icon_field.flags =
      (icon_field.flags & !F_DIGGER_MOVEMASK) | F_DIGGER_DOWN0;

  icon_field.pixmap_id =
    id2iconpixmap_id(icon_field.id, icon_field.flags);

  if (icon_field.pixmap_id != last_icon_pixmap)
    {
      /*wmhints.icon_pixmap = pixmaps[icon_field.pixmap_id];
      wmhints.flags = IconPixmapHint;
      XSetWMHints(display, mainwindow, &wmhints);*/
      /*CopyOnePixmap_Window(iconwindow, icon_field.pixmap_id, 0, 0);*/
      XCopyArea(display, iconpixmaps[icon_field.pixmap_id], iconwindow, gc,
                0, 0, 16*iconmag, 16*iconmag, 0, 0);
      XFlush(display);
      last_icon_pixmap = icon_field.pixmap_id;
    }
  in_handle_icon = False;
} /* void Handle_Icon() */

void Handle_IconWindow_Event(XEvent *event)
{
  switch(event->type)
  {
    case Expose:
      if (event->xexpose.count == 0)
      {
        XCopyArea(display, iconpixmaps[last_icon_pixmap], iconwindow, gc,
                  0, 0, 16*iconmag, 16*iconmag, 0, 0);
      }
      break;
  }
}
