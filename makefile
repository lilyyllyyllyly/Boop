CC=clang
CFLAGS ?= -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -O2
DEBUG=-fsanitize=address,undefined -g3

export CFLAGS

INCLUDE=-IMason/include/ -IMason/Scaffold/include/ -Iinclude
LIBS=-LMason/lib/ -LMason/Scaffold/lib/ -lmason -lscaffold -lraylib

SRC_DIR=src
SOURCES=$(wildcard ${SRC_DIR}/*.c)

LIB_SUBDIRS=Mason/Scaffold Mason/

OUTPUT=boop

.PHONY: final debug enable-debug ${LIB_SUBDIRS}

final: ${LIB_SUBDIRS}
	${CC} ${CFLAGS} ${SOURCES} ${INCLUDE} ${LIBS} -o ${OUTPUT}

${LIB_SUBDIRS}:
	$(MAKE) -C $@

debug: enable-debug final

enable-debug:
	$(eval CFLAGS := ${CFLAGS} ${DEBUG})

