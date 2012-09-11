# $Id: Makefile.in,v 1.9 2006/03/14 22:55:41 djm Exp $

prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
sbindir=${exec_prefix}/sbin
libexecdir=${exec_prefix}/libexec
datadir=${prefix}/share
mandir=${prefix}/share/man
sysconfdir=${prefix}/etc
srcdir=.
top_srcdir=.

CC=gcc
LDFLAGS=
CFLAGS=-g -O2
CPPFLAGS=-I$(srcdir) 
LIBS=-lpcap 
EXEEXT=
INSTALL=/usr/bin/install -c

#CFLAGS+=-DFLOW_RB		# Use red-black tree for flows
CFLAGS+=-DFLOW_SPLAY		# Use splay tree for flows
CFLAGS+=-DEXPIRY_RB		# Use red-black tree for expiry events
#CFLAGS+=-DEXPIRY_SPLAY		# Use splay tree for expiry events

TARGETS=softflowd${EXEEXT} softflowctl${EXEEXT}

COMMON=convtime.o strlcpy.o strlcat.o closefrom.o daemon.o

all: $(TARGETS)

softflowd${EXEEXT}: log.o softflowd.o netflow1.o netflow5.o netflow9.o $(COMMON)
	$(CC) $(LDFLAGS) -o $@ log.o netflow1.o netflow5.o netflow9.o softflowd.o $(COMMON) $(LIBS)

softflowctl${EXEEXT}: softflowctl.o $(COMMON)
	$(CC) $(LDFLAGS) -o $@ softflowctl.o $(COMMON) $(LIBS)

clean:
	rm -f $(TARGETS) *.o core *.core

realclean: clean
	rm -rf autom4te.cache Makefile config.log config.status

distclean: realclean
	rm -f config.h* configure

strip:
	strip $(TARGETS)

install:
	[ -d $(DESTDIR)$(sbindir) ] || \
	    $(srcdir)/mkinstalldirs $(DESTDIR)$(sbindir)
	[ -d $(DESTDIR)$(mandir)/man8 ] || \
	    $(srcdir)/mkinstalldirs $(DESTDIR)$(mandir)/man8
	$(INSTALL) -m 0755 -s softflowd $(DESTDIR)$(sbindir)/softflowd
	$(INSTALL) -m 0755 -s softflowctl $(DESTDIR)$(sbindir)/softflowctl
	$(INSTALL) -m 0644 softflowd.8 $(DESTDIR)$(mandir)/man8/softflowd.8
	$(INSTALL) -m 0644 softflowctl.8 $(DESTDIR)$(mandir)/man8/softflowctl.8
