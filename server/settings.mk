# Libraries
LIBS=commons pthread readline m
LIBDIRS += ../gutils
LIBS    += gutils
IDIRS   += ../gutils




# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=

# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG
