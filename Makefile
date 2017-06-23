# simple makefile for a Rappture-based program

RAPPTURE_DIR	= /usr/local/rappture
INCLUDES	= -I$(RAPPTURE_DIR)/include
LIBS		= -L$(RAPPTURE_DIR)/lib -lrappture -lm

VENDORDIR = ./vendor
OBJDIR = ./obj

include Makefile.inc

all: main

main: main.cpp $(WN_OBJ)
	$(CC) $(CFLAGS) $(WN_OBJ) $(INCLUDES) $< -o $@ $(LIBS) $(CLIBS) -DDEBUG

clean_runs:
	$(RM) run*.xml driver*.xml

clean_rappture: clean_runs
	$(RM) main 

clean_all: clean_rappture
	rm -fr $(VENDORDIR) $(OBJDIR)
