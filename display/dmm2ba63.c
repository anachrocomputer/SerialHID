/* dmm2ba63 --- read from VC99 DMM and write to BA63 disp   2012-05-07 */
/* Copyright (c) 2012 John Honniball, Froods Software Development      */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define MAXLINE (64)

#define ESC (0x1b)


int Fd = -1;
FILE *Dmmfp;


void display(const char *const str)
{
   const int n = strlen(str);
   
   if (write(Fd, str, n) != n)
      perror("write");
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


void cursor_rc(const int row, const int col)
{
   char buf[MAXLINE];
   
   sprintf(buf, "%c[%d;%dH", ESC, row, col);
   
   write(Fd, buf, strlen(buf));
}


void set_code_page(const int page)
{
// display("\x1bR\x03");    // Select UK character set
// display("\x1bR1");       // Select code page 850
// display("\x1bR2");       // Select code page 852
// display("\x1bR4");       // Select code page 858
   char buf[3];
   
   buf[0] = ESC;
   buf[1] = 'R';
   buf[2] = page;
   
   write(Fd, buf, 3);
}


static int openDMMPort(const char *const port)
{
   struct termios tbuf;
   long int fdflags;

   const int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
   
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

   tcgetattr(fd, &tbuf);
   
   cfsetospeed(&tbuf, B2400);
   cfsetispeed(&tbuf, B2400);
   cfmakeraw(&tbuf);

   tbuf.c_cflag |= CLOCAL;
   
   if (tcsetattr(fd, TCSAFLUSH, &tbuf) < 0) {
      perror("tcsetattr");
      return(-1);
   }
   
   if ((Dmmfp = fdopen(fd, "rw")) == NULL) {
      perror("fdopen");
      return(-1);
   }
   
   return (fd);
}


static int openBA63Port(const char *port)
{
   struct termios tbuf;
   long int fdflags;

   const int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
   
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


int getReading(FILE *fp, char *buf)
{
   char *p;
   char lin[MAXLINE];
   
   p = fgets(lin, MAXLINE, fp);

   memcpy(buf, lin, 14);
   
   return (1);
}


void appendSB1(char *str, const int sb1)
{
   if (sb1 & 0x01)
      strcat(str, " BPN");

   if (sb1 & 0x02)
      strcat(str, " HOLD");

   if (sb1 & 0x04)
      strcat(str, " REL");

   if (sb1 & 0x08)
      strcat(str, " AC");

   if (sb1 & 0x10)
      strcat(str, " DC");

   if (sb1 & 0x20)
      strcat(str, " AUTO");

   if (sb1 & 0x40)
      strcat(str, " ???");

   if (sb1 & 0x80)
      strcat(str, " ???");
}


void appendSB2(char *str, const int sb2)
{
   if (sb2 & 0x01)
      strcat(str, " Z3");

   if (sb2 & 0x02)
      strcat(str, " n");

   if (sb2 & 0x04)
      strcat(str, " Bat");

   if (sb2 & 0x08)
      strcat(str, " APO");

   if (sb2 & 0x10)
      strcat(str, " MIN");

   if (sb2 & 0x20)
      strcat(str, " MAX");

   if (sb2 & 0x40)
      strcat(str, " Z2");

   if (sb2 & 0x80)
      strcat(str, " Z1");
}


void appendSB3(char *str, const int sb3)
{
   if (sb3 & 0x01)
      strcat(str, " Z4");

   if (sb3 & 0x02)
      strcat(str, " %");

   if (sb3 & 0x04)
      strcat(str, " Diode");

   if (sb3 & 0x08)
      strcat(str, " Beep");

   if (sb3 & 0x10)
      strcat(str, " M");

   if (sb3 & 0x20)
      strcat(str, " k");

   if (sb3 & 0x40)
      strcat(str, " m");

   if (sb3 & 0x80)
      strcat(str, " \xe6");  /* mu */
}


void appendSB4(char *str, const int sb4)
{
   if (sb4 & 0x01)
      strcat(str, " \xf8" "F");

   if (sb4 & 0x02)
      strcat(str, " \xf8" "C");

   if (sb4 & 0x04)
      strcat(str, " F");

   if (sb4 & 0x08)
      strcat(str, " Hz");

   if (sb4 & 0x10)
      strcat(str, " hFE");

   if (sb4 & 0x20)
      strcat(str, " Ohm");

   if (sb4 & 0x40)
      strcat(str, " Amp");

   if (sb4 & 0x80)
      strcat(str, " Volt");
}


int main(const int argc, const char *const argv[])
{
   unsigned char lin[MAXLINE];
   char str[MAXLINE];
   char modes[MAXLINE];
   char units[MAXLINE];
   char acdc[MAXLINE];
   char blob[2];
   char halfblob[2];
   int barg;
   int i;
   
   blob[0] = 0xdb;
   blob[1] = '\0';

   halfblob[0] = 0xdd;
   halfblob[1] = '\0';

   Fd = openDMMPort("/dev/ttyUSB1");
   Fd = openBA63Port("/dev/ttyUSB0");

   set_code_page(0x34);
   
   clear_screen();

   for (;;) {
      getReading(Dmmfp, lin);
      modes[0] = '\0';
      units[0] = '\0';
      acdc[0] = '\0';
      
      printf("<%s>\n", lin);
      
      if (lin[0] == '-')
         str[0] = '-';
      else
         str[0] = ' ';
      
      str[1] = '\0';
      
      switch (lin[6]) {
      case '0':
         strncat(str, &lin[1], 4);
         strcat(str, ".");
         break;
      case '1':
         strncat(str, &lin[1], 1);
         strcat(str, ".");
         strncat(str, &lin[2], 3);
         break;
      case '2':
         strncat(str, &lin[1], 2);
         strcat(str, ".");
         strncat(str, &lin[3], 2);
         break;
      case '3':
      case '4':
         strncat(str, &lin[1], 3);
         strcat(str, ".");
         strncat(str, &lin[4], 1);
         break;
      }

      appendSB1(modes, lin[7]);
      appendSB2(modes, lin[8]);
      appendSB3(units, lin[9]);
      appendSB4(units, lin[10]);
      
      if (lin[7] & 0x08)
         strcat(acdc, " AC");
      
      if (lin[7] & 0x10)
         strcat(acdc, " DC");
      
      printf("[%s %s %s] %d\n", str, units, modes, lin[11]);
      cursor_home();
      display(str);
      display(units);
      display(acdc);
      clear_to_eol();
      
      cursor_rc(2, 1);
      
      if (lin[7] & 0x01) {
         barg = lin[11] & 0x7f;
         
         for (i = 0; i < (barg / 3); i++)
            display(blob);
            
         if ((barg % 3) == 2)
            display(halfblob);
      }

      clear_to_eol();

      fflush(stdout);
   }
   
   close(Fd);
   
   return (0);
}
