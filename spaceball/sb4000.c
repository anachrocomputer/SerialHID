/* sb4000 --- read data from Spaceball 4000 FLX             2020-04-11 */
/* Copyright (c) 2020 John Honniball, Froods Software Development      */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int Fd = 0;


static int openSB4000Port(const char *port)
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
   
   tbuf.c_cflag |= CLOCAL;
   tbuf.c_cflag &= ~CSIZE;
   tbuf.c_cflag |= CS8 | CREAD;
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


/* SB4000Send --- send a command via serial to the Spaceball 4000 FLX */

void SB4000Send(const char *str)
{
   const int n = strlen(str);
   
   if (write(Fd, str, n) != n)
      perror("write");
}


/* SB4000Recv --- receive a CR-terminated string from the Spaceball 4000 FLX */

int SB4000Recv(char *const str)
{
   int i;
   char ch;
   char buf[2];
   int esc = 0;
   
   i = 0;
   
   do {
//    printf("Before read()...\n");
      if (read(Fd, buf, 1) < 0) {
         perror("read");
         exit(1);
      }

      ch = buf[0];
//    printf("ch = %02x\n", ch);

      if (esc) {
         if (ch == '^')
            ;
         else if (ch == 'M')
            ch = '\r';
         else if (ch == 'Q')
            ch = 0x11;
         else if (ch == 'S')
            ch = 0x13;
         
         esc = 0;
      }
      else if (ch == '^')
         esc = 1;

      if ((esc == 0) && (ch != '\r'))
         str[i++] = ch;
   } while (buf[0] != '\r');

   str[i] = '\0';
   
   return (i);
}


/* Identify --- read ID string from Spaceball and print it */

int Identify(void)
{
   char str1[64];
   char str2[64];

   SB4000Send("@RESET\r");
   
   SB4000Recv(str1);
   printf("ID: '%s'\n", str1);
   SB4000Recv(str1);
   printf("ID: '%s'\n", str1);
   SB4000Recv(str2);
   printf("ID: '%s'\n", str2);
   SB4000Recv(str2);
   printf("ID: '%s'\n", str2);

   return (strlen(str1));
}


/* SB4000Init --- initialise the Spaceball */

int SB4000Init(void)
{
// SB4000Send("CB\r");  /* Binary comms mode */
   SB4000Send("MSS\r");
   
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


/* ReadSB4000Packet --- read a message from the Spaceball */

void ReadSB4000Packet(unsigned char *const buf)
{
   SB4000Recv(buf);
}


void PrintSB4000Packet(const unsigned char *const buf)
{
   char buttons[16];
   unsigned int pp;
   short int xx, yy, zz;
   short int XX, YY, ZZ;
   
   memset(buttons, '.', sizeof (buttons));
   buttons[12] = '\0';
   
   switch (buf[0]) {
   case '.':
      printf("'.': %02x %02x\n", buf[1], buf[2]);
      if (buf[1] & 0x04)
         buttons[0] = 'A';

      if (buf[1] & 0x08)
         buttons[1] = 'B';

      if (buf[1] & 0x10)
         buttons[2] = 'C';

      if (buf[2] & 0x01)
         buttons[3] = '1';

      if (buf[2] & 0x02)
         buttons[4] = '2';

      if (buf[2] & 0x04)
         buttons[5] = '3';

      if (buf[2] & 0x08)
         buttons[6] = '4';

      if (buf[2] & 0x10)
         buttons[7] = '5';

      if (buf[2] & 0x20)
         buttons[8] = '6';

      if (buf[2] & 0x80)
         buttons[9] = '7';

      if (buf[1] & 0x01)
         buttons[10] = '8';

      if (buf[1] & 0x02)
         buttons[11] = '9';

      puts(buttons);
      break;
   case 0x4b:
//    printf("'%c': %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
      break;
   case 'D':
      printf("'%c': %02x %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
      pp = (buf[1] * 256)  + buf[2];
      xx = (buf[3] * 256)  + buf[4];
      yy = (buf[5] * 256)  + buf[6];
      zz = (buf[7] * 256)  + buf[8];
      XX = (buf[9] * 256)  + buf[10];
      YY = (buf[11] * 256) + buf[12];
      ZZ = (buf[13] * 256) + buf[14];
      printf("pp: %d, xx: %d, yy: %d, zz: %d, XX: %d, YY: %d, ZZ: %d\n", pp, xx, yy, zz, XX, YY, ZZ);
      break;
   default:
      printf("%02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
      break;
   }
}


int main (int argc, char *argv[])
{
   int i;
   char buf[64];
   
   /* Open the serial port connection to the SB4000 */
   Fd = openSB4000Port("/dev/ttyS4");
   
   Identify();

   SB4000Init();
   
   while (1) {
      memset(buf, 0, sizeof (buf));
      ReadSB4000Packet(buf);
      PrintSB4000Packet(buf);
   }
   
   
   close(Fd);
   
   return 0;
}
