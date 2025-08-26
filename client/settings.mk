# Libraries
LIBS=commons pthread readline m
LIBDIRS += ../gutils
LIBS    += gutils
IDIRS   += ../gutils


# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG
