SHELL=/bin/bash

ifndef prefix
prefix=/usr/local
endif

ifndef cc
cc=gcc
endif

bindir=$(prefix)/bin

all: radec2xms

#########################################
# ICC DEFS
#
ifeq ($(strip $(cc)),icc)

export LC_CTYPE=C
# C preprocessor flags
CPPFLAGS=

# C Compiler and flags
CC=icc
CFLAGS=-O3 -fast -Ofast $(DEFINES) $(INCLUDES) \
  -g0 -fno-strict-aliasing

# C++ Compiler and flags
CXX=icc
CXXFLAGS=$(CFLAGS)

# Fortran compiler and flags
FC=ifort
FFLAGS=-O3 -ftz

# Loader Flags And Libraries
LD=$(CC)
LDFLAGS = $(CFLAGS)
LDLIBS +=
endif



#########################################
#
# GCC DEFS
#
ifeq ($(strip $(cc)),gcc)

# C preprocessor flags
CPPFLAGS=

# C Compiler and flags
CC=gcc
CFLAGS=-O3 $(DEFINES) $(INCLUDES) \
  -g0 -Wall -Wextra -fexpensive-optimizations -ffast-math -fassociative-math -fno-signed-zeros -fno-strict-aliasing

# C++ Compiler and flags
CXX=gcc
CXXFLAGS=$(CFLAGS)

# Fortran compiler and flags
FC=gfortran
FFLAGS=-O3

# Loader Flags And Libraries
LD=$(CC)
LDFLAGS = $(CFLAGS)
LDLIBS +=
endif



#########################################


$(bindir):
	mkdir -p $@

radec2xms: radec2xms.c
	$(CC) -O3 $^ -o $@ && strip --strip-all $@

clean:
	$(RM) *.o

distclean:
	$(RM) radec2xms


install: $(bindir)
	cp radec2xms $(bindir)

uninstall:
	rm -rf $(bindir)/radec2xms
