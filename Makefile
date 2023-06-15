CC:=gcc
CFLAGS:=-O3 -march=native
ZIPSRC:=include/zip.c
ZIPOBJ:=zip.o

all: zip unzip

.PHONY: $(all) clean clear

zip: $(ZIPOBJ) cli-zip.o
	$(CC) $(CFLAGS) -o $@ $^

unzip: $(ZIPOBJ) cli-unzip.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ZIPOBJ): $(ZIPSRC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o

clear:
	rm -f *.o zip unzip

# vim: noexpandtab
