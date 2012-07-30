CC = gcc
CFLAGS = -g
HDRS = random.h inet.h
SRCS = main.c placement.c degree.c connection.c expansion.c \
       random.c libj_random.c 
MKDEP=/usr/X11R6/bin/makedepend
VERSION = inet-3.0

inet: $(SRCS)
	$(CC) $(CFLAGS) -o inet $(SRCS) -lm

dist:
	- mkdir $(VERSION)
	- (cd $(VERSION);\
	ln -s ../*.[ch] .;\
	ln -s ../Makefile .;\
	ln -s ../inet.1 .;\
	ln -s ../RELEASE.NOTE .;\
	cd ..)
	tar -czhvf $(VERSION).tar.gz $(VERSION)

clean:
	rm -f *.o inet Makefile.bak

depend: $(SRCS) $(HDRS)
	$(MKDEP) $(CFLAGS) $(SRCS) $(HDRS)

# DO NOT DELETE

main.o: /usr/include/unistd.h /usr/include/sys/cdefs.h
main.o: /usr/include/sys/types.h /usr/include/sys/inttypes.h
main.o: /usr/include/machine/ansi.h /usr/include/machine/types.h
main.o: /usr/include/machine/endian.h /usr/include/sys/unistd.h
main.o: /usr/include/sys/_posix.h /usr/include/stdio.h /usr/include/stdlib.h
main.o: /usr/include/math.h random.h inet.h
placement.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
placement.o: /usr/include/machine/ansi.h /usr/include/stdlib.h inet.h
placement.o: random.h
degree.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
degree.o: /usr/include/machine/ansi.h /usr/include/stdlib.h
degree.o: /usr/include/math.h inet.h random.h
connection.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
connection.o: /usr/include/machine/ansi.h /usr/include/stdlib.h inet.h
connection.o: random.h
expansion.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
expansion.o: /usr/include/machine/ansi.h /usr/include/stdlib.h inet.h
expansion.o: random.h
random.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
random.o: /usr/include/machine/ansi.h /usr/include/math.h
