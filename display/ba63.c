/* ba63 --- test the BA63 customer dislay                   2012-05-14 */
/* Copyright (c) 2012 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

   print ("\x1bR\x03");       // Select UK character set
   print ("\x1b[H\x1b[2J");   // Home cursor and clear screen
   print ("UNEXPECTED ITEM IN\r\n");
   print ("BAGGING AREA");
   
   sleep (5);
   
   print ("\x1b[H\x1b[2J");
   print ("NOTES ARE DISPENSED\r\n");
   print ("BELOW THE SCANNER");

   sleep (5);
   
   print ("\x1b[H\x1b[2J");
   print ("404 CREDIT CARD\r\n");
   print ("   NOT FOUND");

   sleep (5);

   print ("\x1b[H\x1b[2J");
   print ("  WARP CORE BREACH\r\n");
   print ("   ** IMMINENT **");

   sleep (5);

   print ("\x1b[H\x1b[2J");
   print ("   ALL YOUR BASE\r\n");
   print ("  ARE BELONG TO US");

   sleep (5);

   print ("\x1b[H\x1b[2J");
   print ("Soon may\r\n");
   print (" the Wellerman come");
   sleep (3);
   print ("\x1b[H\x1b[2J");
   print ("To bring us sugar\r\n");
   print (" and tea and rum");

   sleep (5);

   print ("\x1b[H\x1b[2J");
   print ("   Four Seasons\r\n");
   print (" Total Landscaping");

   sleep (5);

   print ("\x1b[H\x1b[2J");
   print ("        EVER\r\n");
   print ("        GIVEN");

   close (Fd);
   
   return (0);
}
