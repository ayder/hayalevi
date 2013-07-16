CC      = gcc
PROF    = -DLINUX
NOCRYPT =
C_FLAGS = -Wall -ggdb -O3 $(PROF) $(NOCRYPT)
L_FLAGS = -O2 $(PROF) -lcrypt -lsqlite3
# L_FLAGS = -O2 $(PROF)

O_FILES = admin.o banish.o bbs.o board.o command.o command2.o dbase.o editor.o \
	  mail.o message.o resolve.o user.o dict.o

all: bbs

bbs: $(O_FILES)
	rm -f bbs
	$(CC) $(L_FLAGS) -o bbs $(O_FILES)

.c.o: bbs.h
	$(CC) -c $(C_FLAGS) $<

clean:
	-rm -f *.o bbs
test:
	-cp bbs ../lib
	cd ../lib && ./bbs 7777&
	telnet localhost 7777
	

