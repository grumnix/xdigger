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

#define USE_PIPE

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#ifdef USE_PIPE
#else
#include <sys/msg.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include "configure.h"

#ifdef linux
#include <linux/soundcard.h>
#endif
#ifdef SOUND_RPLAY
#include <rplay.h>
#endif
#ifdef SOUND_NAS
#include <audio/audiolib.h>
#include <audio/soundlib.h>
#endif
#include "xdigger.h"
#include "sound.h"

int ton_laenge[3];
unsigned char ton_buffer[3][2000];
int sound_device = SD_AUTO;
Bool soundserver_started = False;

#ifdef USE_PIPE
int filedes[2];
#else
int msgqid;
#endif

#ifdef SOUND_NAS
AuServer *auserver = NULL;
char *aureturn_status;
AuBucketID aubucketids[3];
#endif

void StartSoundServer()
{
  int fork_ok;
  
#ifdef USE_PIPE
  if(pipe(filedes) == -1)
    {
      fprintf(stderr, "%s: No more pipes.\n", progname);
      sound_device = SD_XBELL;
      return;
    }
#else
  struct msqid_ds buf;
  
  msgqid = msgget(IPC_PRIVATE, IPC_CREAT);
  if(msgqid == -1)
    {
      fprintf(stderr, "%s: No more private keys.\n", progname);
      sound_device = SD_XBELL;
      return;
    }
  printf("Got Key %d.\n", msgqid);
  
  msgctl(msgqid, IPC_STAT, &buf);
  buf.msg_perm.mode = 0600;
  msgctl(msgqid, IPC_SET, &buf);
#endif
  
  fork_ok = fork();
  if (fork_ok == -1)
    {
      fprintf(stderr, "%s: fork failed.\n", progname);
      sound_device = SD_XBELL;
#ifdef USE_PIPE
#else
      msgctl(msgqid, IPC_RMID, NULL);
#endif
      return;
    }
  
  if (fork_ok == 0)
    {
      
      /* Das ist der Soundserver */
      
      Bool done = False;
      int fd, i;
#ifdef USE_PIPE
#else
      struct msgbuf msgp;
#endif
      
      for (i = 1; i < pargc; i++) {strcpy(pargv[i], "");}
      strcat(pargv[0], " (Soundserver)");
      if (sound_device == SD_DSP) strcat(pargv[0], " (dsp)");
      if (sound_device == SD_AUDIO) strcat(pargv[0], " (audio)");
      
      /*       strcpy(LastArgv, " (Soundserver)"); */

      while (!done)
	{
	  char ton_typ;
	  
#ifdef USE_PIPE
 	  read(filedes[0], &ton_typ ,1); 
	  if((ton_typ>=0) && (ton_typ<3))
	    {
#else
	  if(msgrcv(msgqid, &msgp, 1, 0, 0) == -1)
	    {
	      /* if (errno != ENOMSG) */
/* 	      perror(progname); */
	      fprintf(stderr, "%s: (msgrcv) %s on id %d.\n",
		      progname, strerror(errno), msgqid);
	      continue;
	    }
	  
	  if(msgp.mtype == 2)
	    {
	      ton_typ = msgp.mtext[0];
#endif
#ifdef linux
	      if (sound_device == SD_DSP)
		{
		  int rate = TON_DSP_RATE;
		  int channels = 1;
		  int bits = 8;
	      
		  fd = open("/dev/dsp", O_WRONLY, 0); 
		  ioctl(fd, SOUND_PCM_WRITE_RATE, &rate);
		  ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &channels);
		  ioctl(fd, SOUND_PCM_WRITE_BITS, &bits);
		}
#endif
	      if (sound_device == SD_AUDIO)
		{
		  fd = open("/dev/audio", O_WRONLY, 0);
		}

	      write(fd, ton_buffer[(int)ton_typ], ton_laenge[(int)ton_typ]);
	      close(fd);
	    }
	  else
	    done = True;
	}
#ifdef linux
      if(sound_device == SD_DSP)
	ioctl(fd, SNDCTL_DSP_RESET, 0);
#endif
      exit(0);
    }
  else
    {
      soundserver_started = True;
    }
} /* StartSoundServer */

void Create_SND_Header(unsigned char *header, unsigned long data_size)
{
  int sample_rate = TON_AUDIO_RATE;
  
  strcpy(header, ".snd");
  header[4] = 0; header[5] = 0; header[6] = 0; header[7] = 24;
  header[8] = data_size >> 24; header[9] = data_size >> 16;
  header[10] = data_size >> 8; header[11] = data_size;
  header[12] = 0; header[13] = 0; header[14] = 0; header[15] = 1;
  header[16] = sample_rate >> 24; header[17] = sample_rate >> 16;
  header[18] = sample_rate >> 8; header[19] = sample_rate;
  header[20] = 0; header[21] = 0; header[22] = 0; header[23] = 1;
}

void Fill_TonBuffer(int ton_low, int ton_high, int rate, Bool sndheader)
{

  int i, j, k, offset;
  unsigned char augenblicklicherpeak;

  if (sndheader) offset = 24; else offset = 0;
  
  /* TON_SCHRITT */
  augenblicklicherpeak=ton_low;
  for (i=0,j=2; j>0; j--)
    {
      for(k=0;k<0x40;k++)
	ton_buffer[TON_SCHRITT][offset+rate*i++/KC_FREQ] =
          augenblicklicherpeak;
      augenblicklicherpeak = ton_low + ton_high - augenblicklicherpeak;
    }
  ton_laenge[TON_SCHRITT] = rate*i/KC_FREQ;
  
  /* TON_STEINE */
  augenblicklicherpeak=ton_low;
  for (i=0,j=0xfc; ; j++,j&=0xff)
    {
      if (j == 0x1c) break;
      for(k=0;k<j;k++)
	ton_buffer[TON_STEINE][offset+rate*i++/KC_FREQ] =
          augenblicklicherpeak;
      augenblicklicherpeak = ton_low + ton_high - augenblicklicherpeak;
    }
  ton_laenge[TON_STEINE] = rate*i/KC_FREQ;
  
  /* TON_DIAMANT */
  augenblicklicherpeak=ton_low;
  for (i=0,j=0x40; j>0; j--)
    {
      for(k=0;k<j;k++)
	ton_buffer[TON_DIAMANT][offset+rate*i++/KC_FREQ] =
          augenblicklicherpeak;
      augenblicklicherpeak = ton_low + ton_high - augenblicklicherpeak;
    }
  ton_laenge[TON_DIAMANT] = rate*i/KC_FREQ;

  if (sndheader)
    {
      Create_SND_Header(ton_buffer[TON_SCHRITT], ton_laenge[TON_SCHRITT]);
      Create_SND_Header(ton_buffer[TON_STEINE], ton_laenge[TON_STEINE]);
      Create_SND_Header(ton_buffer[TON_DIAMANT], ton_laenge[TON_DIAMANT]);
    }
  
} /* Fill_TonBuffer(int ton_low, int ton_high, int korrektur, Bool sndheader)*/

#ifdef linux
Bool Check_DSP(Bool msg)
{
  int fd;
  int rate = TON_DSP_RATE;
  int channels = 1;
  int bits = 8;

  fd = open("/dev/dsp", O_WRONLY, 0);
  if ((fd == -1) ||
      (ioctl(fd, SOUND_PCM_WRITE_RATE, &rate) == -1) ||
      (ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &channels) == -1) ||
      (ioctl(fd, SOUND_PCM_WRITE_BITS, &bits) == -1))
  {
    if (fd != -1) close(fd);
    if (msg || debug)
      fprintf(stderr, "%s: couldn't initialize /dev/dsp\n", progname);
    return(False);
  }
  close(fd);
  sound_device = SD_DSP;
  if (debug) fprintf(stderr, "%s: sound is dsp\n", progname);
  Fill_TonBuffer(TON_DSP_LOW, TON_DSP_HIGH, TON_DSP_RATE, False);
  StartSoundServer();
  return(True);
} /* Check_DSP */
#endif

Bool Check_SUN_Audio(Bool msg)
{
  int fd;

  fd = open("/dev/audio", O_WRONLY, 0);
  if(fd == -1)
  {
    if (msg || debug)
      fprintf(stderr, "%s: couldn't initialize /dev/audio\n", progname);
    return(False);
  }
  close(fd);
  sound_device = SD_AUDIO;
  if (debug) fprintf(stderr, "%s: sound is audio\n", progname);
  Fill_TonBuffer(TON_AUDIO_LOW, TON_AUDIO_HIGH, TON_AUDIO_RATE, False);
  StartSoundServer();
  return(True);
} /* Check_SUN_Audio */

#ifdef SOUND_NAS
Bool Check_NAS(Bool msg)
{
  int i, sample_rate;
  
  auserver = AuOpenServer(display_name, 0, NULL, 0, NULL, NULL);
  if (auserver == NULL)
  {
    if (msg || debug)
      fprintf(stderr, "%s: couldn't initialize NAS\n", progname);
    return(False);
  }
  else
  {
    sample_rate = AuServerMaxSampleRate(auserver);
    if (sample_rate > KC_FREQ) sample_rate = KC_FREQ;
    Fill_TonBuffer(TON_NAS_LOW, TON_NAS_HIGH, sample_rate, False);
    for (i=0; i<3; i++)
    {
      aubucketids[i] = 
        AuSoundCreateBucketFromData(auserver,
                                    SoundCreate(SoundFileFormatSnd,
                                                AuFormatLinearUnsigned8, 1,
                                                sample_rate,
                                                ton_laenge[i], NULL),
                                    ton_buffer[i],
                                    0, NULL, NULL);
      if (aubucketids[i] == AuNone)
      {
        if (msg || debug)
          fprintf(stderr, "%s: AuSoundCreateBucketFromData failed", progname);
        AuCloseServer(auserver);
        return(False);
      }
    }
    sound_device = SD_NAS;
    if (debug) fprintf(stderr, "%s: sound is NAS\n", progname);
    return(True);
  }
}
#endif

Bool Check_XBell(Bool msg)
{
  sound_device = SD_XBELL;
  if (debug) fprintf(stderr, "%s: sound is xbell\n", progname);
  return(True);
}

Bool XDisplay_is_on_Localhost()
{
  char localhost[1024], xhost[1024];
  char *c;
  /*struct hostent localhost_ent, xhost_ent;*/
    
  gethostname(localhost, sizeof(localhost));
  strcpy(xhost, DisplayString(display));
  c = strchr(xhost, ':');
  if (c) *c = 0; else xhost[0] = 0;
  if (strlen(xhost) == 0) return(True);

  strcpy(localhost, gethostbyname(localhost)->h_name);
  strcpy(xhost, gethostbyname(xhost)->h_name);
  if (debug)
    fprintf(stderr, "%s: localhost=%s\n             xhost=%s\n",
            progname, localhost, xhost);
  return(strcmp(localhost, xhost) == 0);
}

void sound_init()
{
  if (sound_device == SD_NONE) return;

  if (sound_device == SD_AUTO)
  {
    if (XDisplay_is_on_Localhost())
    {
      if (debug) fprintf(stderr, "%s: XDisplay is on localhost\n", progname);
#ifdef SOUND_DSP_AUDIO
#ifdef linux
      Check_DSP(False);
#endif
      if (sound_device == SD_AUTO) Check_SUN_Audio(False);
#endif
    }
    else
    {
      if (debug) fprintf(stderr, "%s: XDisplay is not on localhost\n",
                         progname);
    }
#ifdef SOUND_NAS
    if (sound_device == SD_AUTO) Check_NAS(False);
#endif
#ifdef SOUND_RPLAY
#endif
    if (sound_device == SD_AUTO) Check_XBell(False);
    if (sound_device == SD_AUTO)
    {
      sound_device = SD_NONE;
      return;
    }
  }
  else
  {
#ifdef SOUND_DSP_AUDIO
#ifdef linux
    if (sound_device == SD_DSP)
    {
      if (!Check_DSP(True)) sound_device = SD_NONE;
    }
#endif
    if (sound_device == SD_AUDIO)
    {
      if (!Check_SUN_Audio(True)) sound_device = SD_NONE;
    }
#endif
#ifdef SOUND_NAS
    if (sound_device == SD_NAS)
    {
      if (!Check_NAS(True)) sound_device = SD_NONE;
    }
#endif
#ifdef SOUND_RPLAY
#endif
    if (sound_device == SD_XBELL)
    {
      if (!Check_XBell(True)) sound_device = SD_NONE;
    }
  }
}

#ifdef SOUND_RPLAY
int Play_RPlay_Sound(char *soundfile, int volume)
{
  int rplay_fd;
  RPLAY *rp;
  
  rplay_fd = rplay_open_display(); if (rplay_fd == -1) return(-1);
  rp = rplay_create(RPLAY_PLAY); if (rp == NULL) return(-1);

  rplay_set(rp, RPLAY_APPEND, RPLAY_SOUND, soundfile, RPLAY_VOLUME, 
	    volume, RPLAY_PRIORITY, 255, NULL);
  rplay(rplay_fd, rp);

  rplay_close(rplay_fd);
  rplay_destroy(rp);
  return(0);
}
#endif

#ifdef SOUND_NAS
AuFlowID lastflow = -1;

void Play_NAS_Sound(char ton_typ)
{
  if (auserver != NULL)
  {
    if ((0 <= ton_typ) && (ton_typ <= 2))
    {
      /*AuDestroyFlow(auserver, lastflow, NULL);*/
      if (AuSoundPlayFromBucket(auserver, aubucketids[(int)ton_typ], AuNone,
                                AuFixedPointFromSum(1,0),
                                NULL, NULL, 1,
                                &lastflow, NULL, NULL, NULL) == NULL)
        fprintf(stderr, "%s: AuSoundPlayFromBucket failed", progname);
    }
    AuFlush(auserver);
  }
}
#endif

void sound(char ton_typ)
{
#ifdef SOUND_DSP_AUDIO

  if (soundserver_started) 
    {
#ifdef USE_PIPE
      write(filedes[1],&ton_typ,1);
#else
      struct msgbuf msgp;
      
      msgp.mtype = 2;
      msgp.mtext[0] = ton_typ;
      msgsnd(msgqid, &msgp, 1, 0);
#endif
    }
  if (sound_device == SD_XBELL) 
    if (ton_typ == TON_DIAMANT) XBell(display, -50);

#endif

#ifdef SOUND_RPLAY
  if (sound_device == SD_RPLAY)
    {
      char name[256] = XDIGGER_LIB_DIR;
      char error[20];

      switch (ton_typ)
	{
	case TON_DIAMANT:
	  strcat(name, "/diamond.au");
	  break;
	case TON_SCHRITT:
	  strcat(name, "/step.au");
	  break;
	case TON_STEINE:
	  strcat(name, "/stone.au");
	  break;
	}
      
/*       if (rplay_host_volume(hostname, name, 255) < 0) */
/*       if (rplay_display(name) < 0) */
      if (Play_RPlay_Sound(name, 200) < 0)
	{
	  sprintf(error, "%s: (rplay) ", progname);
	  rplay_perror(error);
	  fprintf(stderr, "%s: disable rplay-sound.\n", progname);
	  sound_device = SD_NONE;
	}

    }
#endif

#ifdef SOUND_NAS
  if (sound_device == SD_NAS)
  {
    Play_NAS_Sound(ton_typ);
  }
#endif
}

void sound_done()
{
#ifdef SOUND_DSP_AUDIO

  if (soundserver_started)
    {
#ifdef USE_PIPE
      char beenden = -1;

      write(filedes[1], &beenden, 1);
      wait(0);
#else
      struct msgbuf msgp;

      msgp.mtype = 1;
      msgsnd(msgqid, &msgp, 1, 0);
      wait(0);
      msgctl(msgqid, IPC_RMID, NULL);
#endif
    }
#endif

#ifdef SOUND_RPLAY
#endif

#ifdef SOUND_NAS
  if (sound_device == SD_NAS)
  {
    if (auserver != NULL)
    {
      int i;
      
      for (i=0; i<3; i++)
        AuDestroyBucket(auserver, aubucketids[i], NULL);
      AuCloseServer(auserver);
    }
  }
#endif
} /* sound_done */
