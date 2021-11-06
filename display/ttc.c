/* ttc --- Unix time_t clock for BA63 display               2012-04-28 */
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


void print(const char *str)
{
   int n = strlen (str);
   
   if (write(Fd, str, n) != n)
      perror("write");
}


static int openBA63Port(const char *port)
{
   int fd;
   struct termios tbuf;
   long int fdflags;

   fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
   
   if (fd < 0) {
      perror(port);
      exit(1);
   }
   
   if ((fdflags = fcntl(fd, F_GETFL, NULL)) < 0) {
      perror("fcntl GETFL");
      exit(1);
   }
   
   fdflags &= ~O_NDELAY;
   
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


void clear_screen(void)
{
   char buf[4];
   
   buf[0] = ESC;
   buf[1] = '[';
   buf[2] = '2';
   buf[3] = 'J';
   
   write(Fd, buf, 4);
}


void clear_to_eol(void)
{
   char buf[4];
   
   buf[0] = ESC;
   buf[1] = '[';
   buf[2] = '0';
   buf[3] = 'K';
   
   write(Fd, buf, 4);
}


void cursor_home(void)
{
   char buf[3];
   
   buf[0] = ESC;
   buf[1] = '[';
   buf[2] = 'H';
   
   write(Fd, buf, 3);
}


void cursor_rc(int row, int col)
{
   char buf[16];
   
   sprintf(buf, "%c[%d;%dH", ESC, row, col);
   
   write(Fd, buf, strlen (buf));
}


void set_code_page(int page)
{
// print ("\x1bR\x03");       // Select UK character set
// print ("\x1bR1");       // Select code page 850
// print ("\x1bR2");       // Select code page 852
// print ("\x1bR4");       // Select code page 858
   char buf[3];
   
   buf[0] = ESC;
   buf[1] = 'R';
   buf[2] = page;
   
   write(Fd, buf, 3);
}


int main(int argc, char argv[])
{
   time_t sec;
   char buf[32];
   
   Fd = openBA63Port("/dev/ttyUSB0");

   print("\x1bR\x03");       // Select UK character set
   clear_screen();
   cursor_home();
   print("TIME_T:\r\n");
   
   printf("time_t: %d bytes\n", sizeof (sec));
   
   for (;;) {
      time(&sec);
      sprintf(buf, "%ld\r", sec);
      print(buf);
      
      sleep(1);
   }

   close(Fd);
   
   return (0);
}
