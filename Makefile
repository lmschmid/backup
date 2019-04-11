# Can link with 32, 64 and Mac versions of the library 
#
# Usage (assume its called Makefile): make  (makes all for a particular machine architecture)
#
# note - if you are moving between machine architectures you must first delete your .o files

CC = gcc
CFLAGS = -g -std=gnu99

SRCS = $(shell ls  *.c 2> /dev/null | grep -v backup.c )
OBJS = $(shell ls  *.c 2> /dev/null | grep -v backup.c | sed s/\.c[p]*$$/\.o/ )
HFILES = $(shell ls *.h 2> /dev/null)
FILE = 32

#use the unique library names for each architecture
ARCH = $(shell arch)
OS = $(shell uname -s)
ifeq ("$(OS)", "Linux")
	ifeq ("$(ARCH)", "x86_64")
		FILE = 64
	endif
endif

#handle macs
ifeq ("$(OS)", "Darwin")
	FILE = "Mac"
endif

ALL = backup

all:  $(OBJS) $(ALL)

echo:
	@echo "CFLAGS: " $(CFLAGS)  
	@echo "SRCS: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "HFILES: $(HFILES)"
	@echo "LIBNAME: $(LIBNAME)"

%.o: %.c $(HFILES) 
	@echo "-------------------------------"
	@echo "*** Building $@"
	$(CC) -c $(CFLAGS) $< -o $@

backup: backup.o  $(HFILES) $(OBJS)
	@echo "-------------------------------"
	@echo "*** Building $@"
	$(CC) $(CFLAGS) -lpthread -o $@ backup.o $(OBJS) 
	@echo "-------------------------------"

# clean .o
cleano: 
	@echo "-------------------------------"
	@echo "*** Cleaning Files..."
	@echo "Deleting *.o's only"
	rm -f *.o 
	@echo "-------------------------------"
	
# clean targets
clean: 
	@echo "-------------------------------"
	@echo "*** Cleaning Files..."
	@echo "Deleting *.o's and '$(FILE)' bit versions of backup"
	rm -f *.o $(ALL)
	@echo "-------------------------------"
