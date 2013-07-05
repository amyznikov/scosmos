
TARGET = octave-olss.oct

all: $(TARGET)

SOURCES = kahan.c libogm.c olss.cc
HEADERS = kahan.h libogm.h
MODULES = kahan.o libogm.o olss.o


# Rules for compiling objects

$(MODULES) : $(HEADERS)

CFLAGS = -O3 -g0 -Wall -Wextra
%.o: %.c
	CFLAGS="$(CFLAGS)" $(MKOCTFILE) -o $@ -c $<


CXXFLAGS = -O3 -g0 -Wall -Wextra
%.o: %.cc
	CXXFLAGS="$(CXXFLAGS)" $(MKOCTFILE) -o $@ -c $<


ifndef MKOCTFILE
MKOCTFILE = mkoctfile
endif


$(TARGET): $(MODULES)
	$(MKOCTFILE) -s --verbose -Wall $(MODULES) -o $@ 

clean:
	rm -f *.oct *.o

dist: clean
	tar cfz ../../octave-olss.tar.gz ../../octave-olss && echo "../../octave-olss.tar.gz saved"
