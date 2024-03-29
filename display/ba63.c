/* ba63 --- test the BA63 customer dislay                   2012-05-14 */
/* Copyright (c) 2012 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

int Fd = 0;

#define ESC    (0x1b)


void display(const char *const str)
{
   const int n = strlen(str);
   
   if (write(Fd, str, n) != n)
      perror("write");
}


static int openBA63Port(const char *const port)
{
   struct termios tbuf;
   long int fdflags;

   const int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
   
   if (fd < 0) {
      perror(port);
      exit(1);
   }
   
   if ((fdflags = fcntl(fd, F_GETFL, NULL)) < 0) {
      perror("fcntl GETFL");
      exit(1);
   }
   
   fdflags &= ~O_NONBLOCK;
   
   if (fcntl(fd, F_SETFL, fdflags) < 0) {
      perror("fcntl SETFL");
      exit(1);
   }

   if (tcgetattr(fd, &tbuf) < 0) {
      perror("tcgetattr");
      exit(1);
   }
   
   cfsetospeed(&tbuf, B9600);
   cfsetispeed(&tbuf, B9600);
   cfmakeraw(&tbuf);
   
   tbuf.c_cflag |= PARENB | PARODD | CLOCAL;
   
   if (tcsetattr(fd, TCSAFLUSH, &tbuf) < 0) {
      perror("tcsetattr");
      exit(1);
   }
   
   return (fd);
}


int readBA63(void)
{
   char buf[2];
   
   if (read(Fd, buf, 1) < 0) {
      perror("read");
      exit(1);
   }
   
   return (buf[0]);
}


int main(const int argc, const char *const argv[])
{
   int ch;
   int i;
   char reply[32];
   
   Fd = openBA63Port("/dev/ttyUSB0");

   display("\x1b[0c");         // Enquire what device we're connected to
   
   for (i = 0; ((ch = readBA63()) != 'c') && (i < 30); i++) {
      if (ch == ESC) {
         reply[i++] = 'E';
         reply[i++] = 'S';
         reply[i]   = 'C';
      }
      else
         reply[i] = ch;
   }
   
   reply[i++] = ch;
   reply[i] = '\0';
   
   printf("Reply is: %s\n", reply);
   
   display("\x1bR\x03");       // Select UK character set
   display("\x1b[H\x1b[2J");   // Home cursor and clear screen
   display("UNEXPECTED ITEM IN\r\n");
   display("BAGGING AREA");
   
   sleep(5);
   
   display("\x1b[H\x1b[2J");
   display("NOTES ARE DISPENSED\r\n");
   display("BELOW THE SCANNER");

   sleep(5);
   
   display("\x1b[H\x1b[2J");
   display("404 CREDIT CARD\r\n");
   display("   NOT FOUND");

   sleep(5);

   display("\x1b[H\x1b[2J");
   display("  WARP CORE BREACH\r\n");
   display("   ** IMMINENT **");

   sleep(5);

   display("\x1b[H\x1b[2J");
   display("   ALL YOUR BASE\r\n");
   display("  ARE BELONG TO US");

   sleep(5);

   display("\x1b[H\x1b[2J");
   display("Soon may\r\n");
   display(" the Wellerman come");
   sleep(3);
   display("\x1b[H\x1b[2J");
   display("To bring us sugar\r\n");
   display(" and tea and rum");

   sleep(5);

   display("\x1b[H\x1b[2J");
   display("   Four Seasons\r\n");
   display(" Total Landscaping");

   sleep(5);

   display("\x1b[H\x1b[2J");
   display("        EVER\r\n");
   display("        GIVEN");

   sleep(5);

   display("\x1b[H\x1b[2J");

   close(Fd);
   
   return (0);
}
