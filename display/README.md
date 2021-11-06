# display #

Some C programs to work with the Wincor-Nixdorf BA63 customer display.
This display is a serial device, also powered by 12V via the same
serial cable.

It is unusual in that it requires odd parity in all serial communications.
Hence lines of code like this:

`tbuf.c_cflag |= PARENB | PARODD | CLOCAL;`

Note that some USB-to-serial converter cables fail to generate odd
parity correctly and therefore don't work with this display.
