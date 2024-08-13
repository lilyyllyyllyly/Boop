CC=clang
CFLAGS=-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -O2
DEBUG=-fsanitize=address,undefined -g3

INCLUDE=-IMason/include/ -IMason/Scaffold/include/
LIBS=-LMason/lib/ -LMason/Scaffold/lib/ -lmason -lscaffold -lraylib

SRC_DIR=src
SOURCES=$(wildcard ${SRC_DIR}/*.c)

OUTPUT=boop

final:
	${CC} ${CFLAGS} ${SOURCES} ${INCLUDE} ${LIBS} -o ${OUTPUT}

debug: enable-debug final

enable-debug:
	$(eval CFLAGS := ${CFLAGS} ${DEBUG})

