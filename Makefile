all: wc

wc: wc.c wc.h wc_core.c
	gcc wc.c wc_core.c -g -o wc -lm

clean:
	rm -f wc *~ *#
