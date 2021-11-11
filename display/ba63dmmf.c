/* ba63dmmf --- BA63 display for Derby Mini Maker Faire     2012-06-02 */
/* Copyright (c) 2012 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

typedef unsigned char byte;

int Fd = 0;

const int ESC = 0x1b;


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


int main (int argc, char argv[])
{
   Fd = openBA63Port ("/dev/ttyUSB0");

// print ("\x1bR\x03");       // Select UK character set
   print ("\x1bR\x06");       // Select Italy character set

   for (;;) {
      print ("\x1b[H\x1b[2J");   // Home cursor and clear screen
      
      //      12345678901234567890
      print ("DERBY MINI\r\n");
      print ("MAKER FAIRE 2012");
      
      sleep (5);
      
      print ("\x1b[H\x1b[2J");
      print ("FUN WITH FLAT-BED\r\n");
      print ("PEN PLOTTERS");

      sleep (5);
      
      print ("\x1b[H\x1b[2J");
      print ("JOHN HONNIBALL\r\n");
      print ("BRISTOL HACKSPACE");

      sleep (5);

      print ("\x1b[H\x1b[2J");
      print ("FUN WITH FLAT-BED\r\n");
      print ("PEN PLOTTERS");
      
      sleep (5);

      print ("\x1b[H\x1b[2J");
      print ("@anachrocomputer\r\n"); 
      print ("         #DMMF12");
      
      sleep (5);
   }

   close (Fd);
   
   return (0);
}
