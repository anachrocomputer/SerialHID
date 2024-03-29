/* ttc --- Unix time_t clock for BA63 display               2012-04-28 */
/* Copyright (c) 2012 John Honniball                                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>

#define ESC    (0x1b)

#define HEXTIME   (1)
#define TIME_T    (2)
#define PERCENT   (3)
#define DECIMAL   (4)

int Fd = 0;


static void display(const char *const str)
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
   char buf[16];
   
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


void handler(const int sig)
{
   clear_screen();
   close(Fd);
   exit(1);
}


int main(const int argc, const char *const argv[])
{
   int mode = TIME_T;
   time_t sec;
   struct tm *tm_p;
   struct sigaction action;
   unsigned int seconds_today;
   unsigned int hexonds;
   double percent;
   char buf[32];
   
   if (argc > 1) {
      if (argv[1][0] == '-') {
         switch (argv[1][1]) {
         case 'X':
         case 'x':
            mode = HEXTIME;
            break;
         case 'T':
         case 't':
            mode = TIME_T;
            break;
         case 'P':
         case 'p':
            mode = PERCENT;
            break;
         case 'D':
         case 'd':
            mode = DECIMAL;
            break;
         }
      }
   }
   
   Fd = openBA63Port("/dev/ttyUSB0");

   // Catch SIGINT so that we don't leave a non-updating time on the display
   action.sa_flags = 0;
   sigemptyset(&action.sa_mask);
   action.sa_handler = handler;

   if (sigaction(SIGINT, &action, NULL) < 0) {
      perror("sigaction");
      exit(1);
   }

   set_code_page(3);           // Select UK character set
   clear_screen();
   
   printf("time_t: %lu bytes\n", sizeof (sec));
   
   for (;;) {
      time(&sec);
      tm_p = localtime(&sec);
      seconds_today = (((tm_p->tm_hour * 60) + tm_p->tm_min)  * 60) + tm_p->tm_sec;

      cursor_home();

      strftime(buf, sizeof (buf), "%Y-%m-%d %H:%M:%S\r\n", tm_p);
      display(buf);

      switch (mode) {
      case TIME_T:
         sprintf(buf, "time_t: %lu\r", sec);
         break;
      case HEXTIME:
         hexonds = (seconds_today * 0x10000ULL) / 86400ULL;
         sprintf(buf, "0x%04X\r", hexonds);
         break;
      case PERCENT:
         percent = (seconds_today * 100.0) / 86400.0;
         sprintf(buf, "%2.2f%%\r", percent);
         break;
      case DECIMAL:
         percent = seconds_today / 86400.0;
         sprintf(buf, "%0.4f\r", percent);
         break;
      }

      display(buf);
      
      sleep(1);
   }

   close(Fd);
   
   return (0);
}
