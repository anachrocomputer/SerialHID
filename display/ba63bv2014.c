/* ba63bv2014 --- BV Studios Open Weekend 2014              2014-05-31 */
/* Copyright (c) 2014 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

int Fd = 0;

const int ESC = 0x1b;


void display(const char *str)
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

   fd = open (port, O_RDWR | O_NOCTTY | O_NONBLOCK);
   
   if (fd < 0) {
      perror (port);
      exit (1);
   }
   
   if ((fdflags = fcntl (fd, F_GETFL, NULL)) < 0) {
      perror ("fcntl GETFL");
      exit (1);
   }
   
   fdflags &= ~O_NONBLOCK;
   
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

// display("\x1bR\x03");       // Select UK character set
   display("\x1bR\x06");       // Select Italy character set

   for (;;) {
      display("\x1b[H\x1b[2J");   // Home cursor and clear screen
      
      //      12345678901234567890
      display(" BRISTOL HACKSPACE\r\n");
      display("  BV Studios 2014");
      
      sleep (5);
      
      display("\x1b[H\x1b[2J");
      display(" BRISTOL HACKSPACE\r\n");
      display("  John Honniball");

      sleep (5);

      display("\x1b[H\x1b[2J");
      display("FUN WITH FLAT-BED\r\n");
      display("PEN PLOTTERS");
      
      sleep (5);

      display("\x1b[H\x1b[2J");
      display("@anachrocomputer\r\n");
      display("                ");
      
      sleep (5);
   }

   close (Fd);
   
   return (0);
}
