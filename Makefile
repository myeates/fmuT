# simple makefile for a Rappture-based program

RAPPTURE_DIR	= /home/ubuntu/tools/rappture
INCLUDES	= -I$(RAPPTURE_DIR)/include -I$(VENDORDIR)/libstatmech/0.8/src
INCLUDES2	= -I/usr/include -I/usr/include/libxml2/ $(INCLUDES)
LIBS		= -L$(RAPPTURE_DIR)/lib -lrappture -lm

VENDORDIR = ./vendor
OBJDIR = ./obj

include Makefile.inc

all: main

main: main.cpp 
	$(CXX) -c $(CFLAGS) $< $(INCLUDES2) -o $@ $(LIBS) $(CLIBS)

vendor: $(WN_OBJ)
	$(CC) $(CFLAGS) $(WN_OBJ) $(INCLUDES) $< -o $@ $(LIBS) $(CLIBS) -DDEBUG
clean_runs:
	$(RM) run*.xml driver*.xml

clean_rappture: clean_runs
	$(RM) main 

clean_all: clean_rappture
	rm -fr $(OBJDIR)
