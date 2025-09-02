# Libraries
LIBS=commons pthread readline m

IDIRS   += ../gutils


# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG
