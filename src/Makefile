####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -Wall -O3 -ggdb -DLINUX  
CXXFLAGS = -Wall -O3 -ggdb -DLINUX  
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I.
LINK     = g++
LFLAGS   = -O2
LIBS     = -lcrypt -lsqlite3 -lpthread -lstdc++
AR       = ar cqs
RANLIB   = 
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = ./

####### Files

HEADERS = bbs.h
SOURCES = admin.c \
		banish.c \
		bbs.c \
		board.c \
		command.c \
		command2.c \
		dbase.c \
		dict.c \
		editor.c \
		mail.c \
		message.c \
		resolve.c \
		user.c \
		extras.cpp
OBJECTS = admin.o \
		banish.o \
		bbs.o \
		board.o \
		command.o \
		command2.o \
		dbase.o \
		dict.o \
		editor.o \
		mail.o \
		message.o \
		resolve.o \
		user.o \
		extras.o
DESTDIR  = 
TARGET   = bbs

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: $(TARGET)

$(TARGET):  $(OBJECTS) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

dist: 
	@mkdir -p .tmp/bbs && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(DIST) .tmp/bbs/ && ( cd `dirname .tmp/bbs` && $(TAR) bbs.tar bbs && $(GZIP) bbs.tar ) && $(MOVE) `dirname .tmp/bbs`/bbs.tar.gz . && $(DEL_FILE) -r .tmp/bbs

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

admin.o: admin.c bbs.h

banish.o: banish.c bbs.h

bbs.o: bbs.c bbs.h

board.o: board.c bbs.h

command.o: command.c bbs.h

command2.o: command2.c bbs.h

dbase.o: dbase.c bbs.h

dict.o: dict.c bbs.h

editor.o: editor.c bbs.h

mail.o: mail.c bbs.h

message.o: message.c bbs.h

resolve.o: resolve.c bbs.h

user.o: user.c bbs.h

extras.o: extras.cpp bbs.h

####### Install

install:  

uninstall:  

