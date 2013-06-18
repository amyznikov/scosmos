SHELL=/bin/bash

subdirs = ssa-detection-dump \
          ssa-source-dump \
          ssa-detection-dump-plateids \
          ssa-detection-plate-extract \
          ssa-plate-dump \
          ssa-plate-stats \
          radec2xms \
          ssa-pair-stars

all:
	for f in $(subdirs) ; do $(MAKE) -C $$f $@ || exit 1; done

clean:
	for f in $(subdirs) ; do $(MAKE) -C $$f $@ ; done

distclean:
	for f in $(subdirs) ; do $(MAKE) -C $$f $@ ; done

install:
	for f in $(subdirs) ; do $(MAKE) -C $$f $@ || exit 1; done

uninstall:
	for f in $(subdirs) ; do $(MAKE) -C $$f $@ || exit 1; done

