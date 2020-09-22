/* pcmouse --- read data from Mouse Systems PC mouse        2020-09-22 */
/* Copyright (c) 2020 John Honniball, Froods Software Development      */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


int Fd = 0;


static int openMousePort(const char *port)
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
   
   cfsetospeed(&tbuf, B1200);
   cfsetispeed(&tbuf, B1200);
   cfmakeraw(&tbuf);
   
   tbuf.c_cflag |= CLOCAL;
   tbuf.c_cflag &= ~CSIZE;
   tbuf.c_cflag |= CS8 | CREAD | CSTOPB;
   tbuf.c_cflag &= ~PARENB;
   tbuf.c_cflag &= ~CRTSCTS;
   
   tbuf.c_cc[VMIN] = 1;
   tbuf.c_cc[VTIME] = 0;
   
   if (tcsetattr(fd, TCSAFLUSH, &tbuf) < 0) {
      perror("tcsetattr");
      exit(1);
   }
   
   return (fd);
}


/* MouseSend --- send a command via serial to the Microsoft Mouse */

void MouseSend(const char *str)
{
   const int n = strlen(str);
   
   if (write(Fd, str, n) != n)
      perror("write");
}


/* MouseRecv --- receive a three byte string from the Microsoft Mouse */

int MouseRecv(char *const str)
{
   int i;
   unsigned char ch;
   char buf[2];
   
   for (i = 0; i < 5; i++) {
//    printf("Before read()...\n");
      if (read(Fd, buf, 1) < 0) {
         perror("read");
         exit(1);
      }

      ch = buf[0];
//    printf("ch = %02x\n", ch);
      str[i] = ch;
   }

   return (i);
}


/* Identify --- read ID string from mouse and print it */

int Identify(void)
{
   return (0);
}


/* MouseInit --- initialise the mouse */

int MouseInit(void)
{
   
   return (1);
}


void ShowHex(const unsigned char *buf, const int nBytes)
{
   int i, j;
   unsigned char ch;
   
   for (i = 0; i < nBytes; i += 16) {
      printf("%04x ", i);
      
      for (j = 0; j < 16; j++) {
         if ((i + j) < nBytes) {
            ch = buf[i + j];
         
            printf("%02x ", ch);
         }
         else {
            printf("   ");
         }
      }

      printf(" ");
      
      for (j = 0; j < 16; j++) {
         if ((i + j) < nBytes) {
            ch = buf[i + j];

            if ((ch >= 0x20) && (ch <= 0x7f)) {
               printf("%c", ch);
            }
            else {
               printf(".");
            }
         }
         else {
            printf(" ");
         }
      }
      
      printf("\n");
   }
}


/* ReadMousePacket --- read a message from the mouse */

void ReadMousePacket(unsigned char *const buf)
{
   MouseRecv(buf);
}


void PrintMousePacket(const unsigned char *const buf)
{
   char buttons[8];
   signed char dx, dy;
   
   memset(buttons, '.', sizeof (buttons));

   if ((buf[0] & 0x04) == 0)
      buttons[0] = 'L';
      
   if ((buf[0] & 0x02) == 0)
      buttons[1] = 'M';

   if ((buf[0] & 0x01) == 0)
      buttons[2] = 'R';
      
   buttons[3] = '\0';

// puts(buttons);

   dx = buf[1];
   dy = buf[2];

   if (dx & 0x40)
      dx |= 0x80;
      
   if (dy & 0x40)
      dy |= 0x80;
   
   printf("%s dx: %3d, dy: %3d\n", buttons, dx, dy);
}


int main (int argc, char *argv[])
{
   char buf[8];
   
   /* Open the serial port connection to the mouse */
   Fd = openMousePort("/dev/ttyS4");
   
   Identify();

   MouseInit();
   
   while (1) {
      memset(buf, 0, sizeof (buf));
      ReadMousePacket(buf);
      PrintMousePacket(buf);
   }
   
   close(Fd);
   
   return (0);
}
