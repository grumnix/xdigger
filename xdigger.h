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

#ifndef _XDIGGER_H

#define _XDIGGER_H

#include <X11/Xlib.h>

#define VERSION "1.0.10"

#define main_win_title "XDigger"
#define border_width 5

#ifdef linux
#define SHORTTAKT
#endif

#ifdef SHORTTAKT
  #define TAKT_LENGTH 10000
#else
  #define TAKT_LENGTH 100000/*40000*/
#endif

#ifdef SHORTTAKT
  #define STEP_MAX  12
  #define STEP_HALF  5
  #define BLINK_MAX  3
#else
  #define STEP_MAX   1/*2*/
  #define STEP_HALF  0/*1*/
  #define BLINK_MAX  0/*1*/
#endif

#define KCF_SCHWARZ   "#030205"
#define KCF_BLAU      "#0A05B6"
#define KCF_ROT       "#CA0F19"
#define KCF_PURPUR    "#CA1CBB"
#define KCF_GRUEN     "#2ED157"
#define KCF_TUERKIS   "#2DE7C0"
#define KCF_GELB      "#E7E95D"
#define KCF_WEISS     "#F3F0F9"
/*#define KCF_SCHWARZ_2 "#"*/
#define KCF_VIOLETT   "#670FC7"
#define KCF_ORANGE    "#CD871B"
#define KCF_PURPURROT "#CF1178"
#define KCF_GRUENBLAU "#27DD9A"
#define KCF_BLAUGRUEN "#276BD3"
#define KCF_GELBGRUEN "#8FE041"
/*#define KCF_WEISS_2   "#"*/

#define KCB_SCHWARZ   "#030205"
#define KCB_BLAU      "#04028F"
#define KCB_ROT       "#920205"
#define KCB_PURPUR    "#870280"
#define KCB_GRUEN     "#027E13"
#define KCB_TUERKIS   "#028965"
#define KCB_GELB      "#7A7F04"
#define KCB_WEISS     "#7D7C80"

extern char *level_filename;
extern int pargc;
extern char **pargv;
extern char *LastArgv;       /* end of argv */
extern Atom protocol[1];
extern char *display_name;
extern Display *display;
extern int screen;
extern Window rootwindow, mainwindow, toolswindow;
extern GC gc;
extern Colormap colormap;

extern char progname[];
extern int xpmmag, iconmag;
extern Bool vert240;
extern Bool laszlo;  /* especially for Laszlo (Hi Laszlo!) */
extern int vertvar;
extern Bool iconmove;
extern Bool blink;
extern Bool use_standardcolor;
extern struct itimerval timervalue;
extern Bool takt;
/* extern Bool sigterm; */
extern int kcf_tuerkis, kcf_gelb, kcf_weiss, kcb_blau, kcb_rot, kcb_tuerkis;
extern Bool quit, back;
extern Bool in_xdigger_idle;
extern Bool ton_init_OK;
extern Bool debug;

extern int MyRand(int rand_max);
extern void XDigger_Idle();

#endif /* _XDIGGER_H */
