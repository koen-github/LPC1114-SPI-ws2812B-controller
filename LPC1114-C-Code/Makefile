#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2013
# 
#############################################################################

# Specify the project name, this is also the main .cpp file to compile
PROJECT    :=  main

# Specify target chip or board or system
TARGET     := lpc1114fn28

# Specify the location of the bmptk library
BMPTK      := ../../../../

# Specify project files (other than $(PROJECT).*) (if any)
OBJECTS    := 
SOURCES    := sw-uart.c
HEADERS    := sw-uart.h
LIBRARIES  := timer

# specify where project files are (if not in the current directory)
SEARCH    := ../../KoenLibrary

# specify (extra) results you want to get (if any)
RESULTS    := 


# The Makefile.inc does all the work
include $(BMPTK)/Makefile.inc
