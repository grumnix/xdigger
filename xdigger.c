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

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "configure.h"
#include "drawpixmaps.h"
#include "drawtext.h"
#include "highscore.h"
#include "icon.h"
#include "runlevels.h"
#include "title.h"
#include "sound.h"
#include "xdigger.h"

char *level_filename = NULL;
int pargc;
char **pargv;
char progname[256];
char *LastArgv = NULL;       /* end of argv */
char *display_name = NULL;

Atom protocol[1];
Display *display;
int screen;
Window rootwindow, mainwindow, toolswindow = 0;
GC gc;
XStandardColormap xstandardcolormap;
Colormap colormap;
int kcf_tuerkis, kcf_gelb, kcf_weiss, kcb_blau, kcb_rot, kcb_tuerkis;
int xpmmag = 2, iconmag = 3;
Bool vert240 = False;
Bool laszlo = False; /* especially for Laszlo (Hi Laszlo!) */
int vertvar;
Bool iconmove = True;
Bool blink = True;
Bool use_standardcolor = False;
struct itimerval timervalue = {{0, 0},{0, TAKT_LENGTH}};
Bool takt = False, icontakt = False;
/* Bool sigterm = False; */
Bool quit = False, back = False;
Bool in_xdigger_idle = False;
Bool debug = False;

static int signal_list[] = {SIGINT, SIGQUIT, SIGILL, SIGTERM,
			    SIGSEGV, 0};

int zaehler;

void all_done(int s)
{
  sound_done();
  if(s == 0)
    {
    }
  else
    {
      switch (s)
      {
        case SIGILL:
        case SIGSEGV:
          /*fprintf(stderr, "Program received signal %d.\n", s);*/
          psignal(s, progname);
      }
    }
  exit(0);
}

int ErrorHandler(Display *display, XErrorEvent *xerrorevent)
{
  char buffer[1024];
  
  XGetErrorText(display, xerrorevent->error_code, buffer, sizeof(buffer));
  fprintf(stderr, "%s: xlib: %s\n", progname, buffer);
  return 0;
}

int IOErrorHandler(Display *display)
{
  fprintf(stderr, "%s: connection to X server lost.\n", progname);
  all_done(0);
  return 0;
}

void TimerHandler()
{
  takt = True;
  signal(SIGALRM, &TimerHandler);

  if (!in_runlevels || in_LevelEditor)
    setitimer(ITIMER_REAL, &timervalue ,0);

  if (++zaehler == STEP_MAX) zaehler = 0;
  /*if ((zaehler == 1) && iconmove && (!in_handle_icon)) Handle_Icon();*/
  if (zaehler == 1) icontakt = True;
} /* TimerHandler() */

int My_USleep(unsigned long usec)
{
  fd_set readfds, wrfds, exfds;
  struct timeval timeout;
  unsigned long mask;
  int nfds, nfound;

  nfds = ConnectionNumber(display);
  mask = (1 << nfds);

  readfds.fds_bits[0] = mask;
  wrfds.fds_bits[0] = exfds.fds_bits[0] = 0;

  timeout.tv_sec = 0;
  timeout.tv_usec = usec;
  nfound = select(nfds, &readfds, &wrfds, &exfds, &timeout);
  return(nfound);
} /* int My_USleep(unsigned long usec) */

void XDigger_Idle()
{
  in_xdigger_idle = True;
  My_USleep(1000);
  if (icontakt)
  {
    icontakt = False;
    if (iconmove && (!in_handle_icon)) Handle_Icon();
  }
  in_xdigger_idle = False;
} /* void XDigger_Idle() */

int MyRand(int rand_max)
{
  int i;

  i = rand() % rand_max;
  /* i = ((rand() / RAND_MAX) * rand_max) */
  return (i);
} /* int MyRand(int rand_max) */

Bool File_Exists(char *filename)
{
  FILE *f;
  
  if ((f = fopen(filename, "r")) == NULL) return False;
  fclose(f);
  return True;
}

Bool Determine_Level_Filename()
{
  FILE *f;

  if (level_filename == NULL)
  {
    level_filename = malloc(256);
    strcat(strcpy(level_filename, XDIGGER_LIB_DIR), "/xdigger.level");
    if ((f = fopen(level_filename, "r")) == NULL)
    {
      fprintf(stderr, "%s: can't open %s\n", progname, level_filename);
      strcpy(level_filename, progname); strcat(level_filename, ".level");
      fprintf(stderr, "%s: try %s... ", progname, level_filename);
      if ((f = fopen(level_filename, "r")) == NULL)
      {
        fprintf(stderr, "failed.\n");
        return(False);
      }
      else fprintf(stderr, "ok.\n");
    }
  }
  else
  {
    if ((f = fopen(level_filename, "r")) == NULL)
    {
      fprintf(stderr, "%s: can't open %s\n", progname, level_filename);
      return(False);
    }
  }
  fclose(f);
  return(True);
}
     
void MakeWindow(int argc, char **argv)
{
  XSizeHints main_win_sizehints;
  XClassHint classhint;
  int main_win_width, main_win_height;
/*   XSetWindowAttributes winattr; */
/*   int i; */

  main_win_width = 320 * xpmmag;
  main_win_height = (240 + vertvar*16) * xpmmag;

  display = XOpenDisplay(display_name);
  if (display == NULL)
    {
      if(display_name == NULL)
	fprintf(stderr, "%s: can't open display (null)\n", progname);
      else
	fprintf(stderr, "%s: can't open display %s\n", progname, display_name);
      exit(1);
    }
  screen = DefaultScreen(display);

  rootwindow = DefaultRootWindow(display);
  mainwindow = XCreateSimpleWindow(display, rootwindow, 0, 0,
				   main_win_width, main_win_height,
				   border_width, 
				   WhitePixel(display, screen),
				   BlackPixel(display, screen));

  main_win_sizehints.width = main_win_width;
  main_win_sizehints.height = main_win_height;
  main_win_sizehints.min_width = 320; /*main_win_width;*/
  main_win_sizehints.min_height = (240 + vertvar*16); /*main_win_height;*/
  /*main_win_sizehints.max_width = main_win_width;*/
    /*main_win_sizehints.max_height = main_win_height;*/
  main_win_sizehints.width_inc = 320;
  main_win_sizehints.height_inc = (240 + vertvar*16);
  main_win_sizehints.min_aspect.x = 20;
  main_win_sizehints.min_aspect.y = (15 + vertvar);
  main_win_sizehints.max_aspect.x = main_win_sizehints.min_aspect.x;
  main_win_sizehints.max_aspect.y = main_win_sizehints.min_aspect.y;
  main_win_sizehints.flags = PSize | PMinSize /*| PMaxSize*/ | PResizeInc | PAspect;

  XSetStandardProperties(display, mainwindow, main_win_title, main_win_title, 
			 None, argv, argc, &main_win_sizehints);
  /*XSetWMNormalHints(display, mainwindow, &main_win_sizehints);*/
  /*winattr.colormap = xstdclrmap.colormap;*/
  /*XChangeWindowAttributes(display, mainwindow, CWColormap, &winattr);*/
  classhint.res_name =  "xdigger";
  classhint.res_class = "XDigger";
  XSetClassHint(display, mainwindow, &classhint);
} /* MakeWindow(int argc, char **argv) */

void MakeColorsAndGCs()
{
  XGCValues gcvalues;
  unsigned long gcvaluemask;
  XColor color;
/*   Status status; */
/*   int count; */

/*   status = XGetStandardColormap(display, rootwindow, &xstandardcolormap, */
/* 				XA_RGB_DEFAULT_MAP); */
/*   xstandardcolormap = XAllocStandardColormap(); */
/*   xstandardcolormap = NULL; */
/*   count = 1; */
/*   status = XGetRGBColormaps(display, mainwindow, &xstandardcolormap, &count, */
/* 			    XA_RGB_COLOR_MAP); */
/*   if (status == 0) */
/*     fprintf(stderr, "Can't get standard colormap."); */

  colormap = DefaultColormap(display, screen);
  XParseColor(display, colormap, KCF_TUERKIS, &color);
  XAllocColor(display, colormap, &color);
  kcf_tuerkis = color.pixel;
  XParseColor(display, colormap, KCF_GELB, &color);
  XAllocColor(display, colormap, &color);
  kcf_gelb = color.pixel;
  XParseColor(display, colormap, KCF_WEISS, &color);
  XAllocColor(display, colormap, &color);
  kcf_weiss = color.pixel;
  XParseColor(display, colormap, KCB_BLAU, &color);
  XAllocColor(display, colormap, &color);
  kcb_blau = color.pixel;
  XParseColor(display, colormap, KCB_ROT, &color);
  XAllocColor(display, colormap, &color);
  kcb_rot = color.pixel;
  XParseColor(display, colormap, KCB_TUERKIS, &color);
  XAllocColor(display, colormap, &color);
  kcb_tuerkis = color.pixel;

  gcvalues.foreground = WhitePixel(display, screen);
  gcvalues.background = BlackPixel(display, screen);
  gcvaluemask = GCForeground | GCBackground;
  gc = XCreateGC(display, mainwindow, gcvaluemask, &gcvalues);

  gc_text_values.foreground = 0;
  gc_text_values.background = 0;
  gc_text_mask = GCForeground | GCBackground;
  gc_text = XCreateGC(display, mainwindow, gc_text_mask, &gc_text_values);
} /* MakeColorsAndGCs() */

void create_audiofiles()
{
  int fd;

  Fill_TonBuffer(TON_AUDIO_LOW, TON_AUDIO_HIGH, TON_AUDIO_RATE, True);

  fd = open("audio/diamond.au", O_CREAT | O_WRONLY);
  fchmod(fd, 0644);
  write(fd, &ton_buffer[TON_DIAMANT], ton_laenge[TON_DIAMANT]+24);
  close(fd);

  fd = open("audio/stone.au", O_CREAT | O_WRONLY);
  fchmod(fd, 0644);
  write(fd, &ton_buffer[TON_STEINE], ton_laenge[TON_STEINE]+24);
  close(fd);

  fd = open("audio/step.au", O_CREAT | O_WRONLY);
  fchmod(fd, 0644);
  write(fd, &ton_buffer[TON_SCHRITT], ton_laenge[TON_SCHRITT]+24);
  close(fd);
  exit(0);
}

void Version()
{
  printf("XDigger Version %s\n", VERSION);
  exit(1);
} /* Version() */

void Help()
{
  printf("Usage: xdigger [-display display] [-1|2|3|...] [-noblink] [-noiconmove] [-vert240] [-sound ");
#ifdef SOUND_DSP_AUDIO
#ifdef linux
  printf("dsp|");
#endif
  printf("audio|");
#endif
  printf("xbell|");
#ifdef SOUND_RPLAY
  printf("rplay|");
#endif
#ifdef SOUND_NAS
  printf("nas|");
#endif
  printf("auto|off] [-v] [-h]\n");
  /*printf("       where n is a positive integer (default is 2)\n");*/
  exit(1);
} /* Help() */

int main(int argc, char **argv)
{
  int i, m = 0;
  char c;
  /*XWMHints wmhints;*/

  pargc = argc;
  pargv = argv;
  strcpy(progname, argv[0]);
  LastArgv = argv[argc - 1] + strlen(argv[argc - 1]);

  for (i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-display")) display_name = argv[++i]; else
      if ((!strcmp(argv[i], "-s")) ||
          (!strcmp(argv[i], "-sound")) ||
          (!strcmp(argv[i], "--sound")))
          {
            i++;
            if(i == argc) Help();
            if(!strcmp(argv[i], "auto")) sound_device = SD_AUTO; else
#ifdef SOUND_RPLAY
            if(!strcmp(argv[i], "rplay")) sound_device = SD_RPLAY; else
#endif
            if(!strcmp(argv[i], "nas"))
#ifdef SOUND_NAS
              sound_device = SD_NAS; else
#else
            {
              fprintf(stderr, "%s: edit configure.h and recompile "
                      "xdigger for NAS-support\n", progname);
              exit(1);
            }
#endif            
#ifdef SOUND_DSP_AUDIO
#ifdef linux
            if(!strcmp(argv[i], "dsp")) sound_device = SD_DSP; else
#endif
            if(!strcmp(argv[i], "audio")) sound_device = SD_AUDIO; else
#endif
            if(!strcmp(argv[i], "xbell")) sound_device = SD_XBELL; else
            if(!strcmp(argv[i], "off") ||
               !strcmp(argv[i], "none")) sound_device = SD_NONE; else Help();
          } else
      if (!strcmp(argv[i], "--nosound")) sound_device = SD_NONE; else
      if (!strcmp(argv[i], "-iconmove") ||
	  !strcmp(argv[i], "--iconmove")) iconmove = True; else
      if (!strcmp(argv[i], "-noiconmove") ||
	  !strcmp(argv[i], "--noiconmove")) iconmove = False; else
      if (!strcmp(argv[i], "-standardcolor") ||
	  !strcmp(argv[i], "--standardcolor")) use_standardcolor = True; else
      if (!strcmp(argv[i], "-vert240") ||
	  !strcmp(argv[i], "--vert240")) vert240 = True; else
      if (!strcmp(argv[i], "-blink") ||
	  !strcmp(argv[i], "--blink")) blink = True; else
      if (!strcmp(argv[i], "-noblink") ||
	  !strcmp(argv[i], "--noblink")) blink = False; else
      if (!strcmp(argv[i], "--laszlo")) laszlo = True; else
      if ( sscanf(argv[i], "-%d", &m) && (m > 0)) xpmmag = m; else
      if (!strcmp(argv[i], "-v") ||
          !strcmp(argv[i], "--version")) Version(); else
      if (!strcmp(argv[i], "-?") ||
	  !strcmp(argv[i], "-h") ||
	  !strcmp(argv[i], "--help")) Help(); else
      if (!strcmp(argv[i], "--create_audiofiles")) create_audiofiles(); else
      if (!strcmp(argv[i], "-f") ||
          !strcmp(argv[i], "-levelfilename") ||
          !strcmp(argv[i], "--levelfilename"))
        level_filename = strdup(argv[++i]); else
      if (!strcmp(argv[i], "-d")) debug = True; else
      if ((argv[i][0] != '-') && (File_Exists(argv[i])))
        level_filename = strdup(argv[i]);
    }

  if (vert240) vertvar = 0; else vertvar = 1;
  /*if (xpmmag != 2) iconmove = False;*/

  if (!Determine_Level_Filename()) exit(1);

  XSetErrorHandler(ErrorHandler);
  XSetIOErrorHandler(IOErrorHandler);
  MakeWindow(argc, argv);
  MakeIconWindow();
  MakeColorsAndGCs();
  MakeXPixmaps(True);
  ClearCharField(BlackPixel(display, screen), False);

  /*wmhints.icon_pixmap = main_win_icon_pixmap;
  wmhints.flags = IconPixmapHint;
  XSetWMHints(display, mainwindow, &wmhints);*/

  XSelectInput(display, mainwindow, ButtonPressMask | ButtonMotionMask |
               KeyPressMask | KeyReleaseMask | ExposureMask);
  XMapRaised(display, mainwindow);

  protocol[0] = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, mainwindow, protocol, 1);

  sound_init();

  /* Timer initialisieren & anstossen */
  signal(SIGALRM, &TimerHandler);
  {
    int i;
    for(i=0; signal_list[i] != 0; i++)
      signal(signal_list[i], &all_done);
  }
  setitimer(ITIMER_REAL, &timervalue ,0);

  while (!quit)
    {
      c = Title();
      if (quit) break;
      if (c == 'p')
	{
	  RunLevels(1, -1);
	  if (quit) break;
	  if (!back) HighScore(score);
	}
      if (c == 'l')
	{
	  ALookAtTheRooms();
	  if (quit) break;
	}
      if (c == 'e')
	{
	  LevelEditor();
	  if (quit) break;
	}
    }
  all_done(0);
  return 0;
} /* main(int argc, char **argv) */
