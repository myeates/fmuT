# simple makefile for a Rappture-based program

RAPPTURE_DIR	= /home/ubuntu/tools/rappture
INCLUDES2	= -I$(RAPPTURE_DIR)/include -I$(VENDORDIR)/libstatmech/0.8/src
INCLUDES	= -I/usr/include -I/usr/include/libxml2 $(INCLUDES2)
LIBS		= -L$(RAPPTURE_DIR)/lib -lrappture -lm

VENDORDIR = ./vendor
OBJDIR = ./obj

include Makefile.inc

all: main

main: main.cpp $(WN_OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(WN_OBJ) $(LIBS) $(CLIBS)

clean_runs:
	$(RM) run*.xml driver*.xml

clean_rappture: clean_runs
	$(RM) main 

clean_all: clean_rappture
	rm -fr $(OBJDIR)
