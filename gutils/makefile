# Compilador y flags
CC = gcc
CFLAGS = -Wall -g -I.

# Archivos
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
LIB = libgutils.a

# Regla por defecto: compilar la librería
all: $(LIB)

# Crear la librería estática
$(LIB): $(OBJS)
	ar rcs $@ $^

# Compilar objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar
clean:
	rm -f $(OBJS) $(LIB)

.PHONY: all clean
