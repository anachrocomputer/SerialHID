# Makefile for the display-driving programs

CFLAGS= -Wunused-variable

EXE=ba63 ttc ba63chars dmm2ba63 ba63dmmf ba63bmmf bmmf13 mfuk13 \
    ba63bblug bv13 ba63bv2014

all: $(EXE)

clean:
	rm $(EXE)

ba63: ba63.c
	gcc $(CFLAGS) -o ba63 ba63.c

ttc: ttc.c
	gcc $(CFLAGS) -o ttc ttc.c

ba63chars: ba63chars.c
	gcc $(CFLAGS) -o ba63chars ba63chars.c

dmm2ba63: dmm2ba63.c
	gcc $(CFLAGS) -o dmm2ba63 dmm2ba63.c

ba63dmmf: ba63dmmf.c
	gcc $(CFLAGS) -o ba63dmmf ba63dmmf.c

ba63bmmf: ba63bmmf.c
	gcc $(CFLAGS) -o ba63bmmf ba63bmmf.c

bmmf13: bmmf13.c
	gcc $(CFLAGS) -o bmmf13 bmmf13.c

mfuk13: mfuk13.c
	gcc $(CFLAGS) -o mfuk13 mfuk13.c

ba63bblug: ba63bblug.c
	gcc $(CFLAGS) -o ba63bblug ba63bblug.c

bv13: bv13.c
	gcc $(CFLAGS) -o bv13 bv13.c

ba63bv2014: ba63bv2014.c
	gcc $(CFLAGS) -o ba63bv2014 ba63bv2014.c
