PREFIX	= /usr/local

BIN	= vnoded vcmd netns
ALL	= $(BIN) vcmdmodule

CFLAGS	= -Wall -fno-strict-aliasing -O3
LDLIBS	= -lev

VNODESRC= vnode_cmd.c vnode_msg.c vnode_io.c vnode_chnl.c

.PHONY: all vcmdmodule install clean

all:	$(ALL)

vnoded: vnoded_main.c vnode_server.c netns.c $(VNODESRC)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

vcmd: vcmd_main.c vnode_client.c $(VNODESRC)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

netns: netns_main.c netns.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

vcmdmodule:
	python setup.py build

install:
	install $(BIN) $(PREFIX)/sbin
	python setup.py install

clean:
	rm -f $(BIN)
	python setup.py clean --all
