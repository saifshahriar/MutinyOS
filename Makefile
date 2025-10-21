include config.mk

# Sources
SRC = shell.c \
	  cmd.c \
	  calc.c \
	  help.c \
	  date.c \
	  global.c \
	  hostname.c \
	  whoami.c \
	  credit.c
OBJ = ${SRC:.c=.o}
TARGET = mutinyos

# Library sources
LIBSRC = lib/tinyexpr.c lib/compatibility.c
LIBOBJ = ${LIBSRC:.c=.o}

# All objects
OBJ += ${LIBOBJ}

all: mutinyos

%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

lib/%.o: lib/%.c
	${CC} -c -w -std=c99 $< -o $@

${OBJ}: config.h config.mk

mutinyos: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f shell ${OBJ} shell-${VERSION}.tar.gz mutiny mutinyos mutinyos-${VERSION}.tar.gz

run: $(TARGET)
	./$(TARGET)

readme:
	./scc-hook.readme.sh

.PHONY: all clean run readme
