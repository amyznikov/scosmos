SHELL=/bin/bash

ifndef prefix
prefix=/usr/local
endif

bindir=$(prefix)/bin

all: ccut

$(bindir):
	mkdir -p $@

clean:
distclean:

install: $(bindir)
	cp ccut $(bindir)

uninstall:
	rm -rf $(bindir)/ccut
