/* ba63chars --- explore the BA63 character set             2012-05-14 */
/* Copyright (c) 2012 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

int Fd = 0;

const int ESC = 0x1b;
const int GS = 0x1d;


void print (const char *str)
{
   int n = strlen (str);
   
   if (write (Fd, str, n) != n)
      perror ("write");
}


static int openBA63Port (const char *port)
{
   int fd;
   struct termios tbuf;
   long int fdflags;

   fd = open (port, O_RDWR | O_NOCTTY | O_NDELAY);
   
   if (fd < 0) {
      perror (port);
      exit (1);
   }
   
   if ((fdflags = fcntl (fd, F_GETFL, NULL)) < 0) {
      perror ("fcntl GETFL");
      exit (1);
   }
   
   fdflags &= ~O_NDELAY;
   
   if (fcntl (fd, F_SETFL, fdflags) < 0) {
      perror ("fcntl SETFL");
      exit (1);
   }

   if (tcgetattr (fd, &tbuf) < 0) {
      perror ("tcgetattr");
      exit (1);
   }
   
   cfsetospeed (&tbuf, B9600);
   cfsetispeed (&tbuf, B9600);
   cfmakeraw (&tbuf);
   
   tbuf.c_cflag |= PARENB | PARODD | CLOCAL;
   
   if (tcsetattr (fd, TCSAFLUSH, &tbuf) < 0) {
      perror ("tcsetattr");
      exit (1);
   }
   
   return (fd);
}


int safe_char (int ch)
{
   switch (ch) {
   case '\0':
   case '\r':
   case '\n':
   case 27:
      ch = ' ';
      break;
   }
   
   return (ch);
}

int main (int argc, char argv[])
{
   int i;
   char str[64];
   int ch;
   char buf[32];
   
   Fd = openBA63Port ("/dev/ttyUSB0");

// print ("\x1bR\x03");       // Select UK character set
// print ("\x1bR1");       // Select code page 850
// print ("\x1bR2");       // Select code page 852
   print ("\x1bR4");       // Select code page 858
   print ("\x1b[H\x1b[2J");   // Home cursor and clear screen
   
   for (ch = 0; ch < 256; ch += 32) {
      for (i = 0; i < 16; i++)
         buf[i] = safe_char (ch + i);
      
      buf[16] = '\0';
      
      sprintf (str, "%02x: %s\r\n", ch, buf);
      print (str);
   
      for (i = 0; i < 16; i++)
         buf[i] = safe_char (ch + i + 16);
      
      buf[16] = '\0';
      
      sprintf (str, "%02x: %s\r", ch + 16, buf);
      print (str);
   
      sleep (2);
   }
   
   for (i = 0; i < 20; i++) {
      buf[i] = 0xdb;
   }
   
   buf[20] = 0;
   
   print (buf);
   print ("\r\n");
   print (buf);

   close (Fd);
   
   return (0);
}
