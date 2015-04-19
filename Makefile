CFLAGS= -Wall -g
OUTDIR= ./bin/

all: sw csc index

sw: sw.c
	gcc $(CFLAGS) -o $(OUTDIR)sw sw.c

csc: csc.c
	gcc $(CFLAGS) -o $(OUTDIR)csc csc.c

index: index.c
	gcc $(CFLAGS) -o $(OUTDIR)index index.c
