# Linux makefile version $Id: Makefile.linux,v 1.1 1999/05/12 14:39:10 dbt93 Exp $

all: bibcursed

# Where to install
INSTDIR=/usr/local/bin
MANDIR=/usr/man/man1

# Curses library
CURSES=ncurses

bibcursed: bibcursed.c
	gcc -O -o bibcursed bibcursed.c -l$(CURSES)

install: bibcursed
	@if [ -d $(INSTDIR) ]; \
		then \
                echo "Installing binary in $(INSTDIR)";\
		cp bibcursed $(INSTDIR);\
		chmod 755 $(INSTDIR)/bibcursed;\
	else \
		echo "Sorry, $(INSTDIR) does not exist";\
	fi
	@if [ -d $(MANDIR) ]; \
		then \
		echo "Installing manual page in $(MANDIR)";\
		cp bibcursed.1 $(MANDIR);\
		chmod 644 $(MANDIR)/bibcursed.1;\
	else \
		echo "$(MANDIR) does not exist";\
	fi

clean:
	rm -f bibcursed
