CC = gcc
OUTPUT = main
SOURCE = main.c

all:
	${CC} ${SOURCE} -o ${OUTPUT}

cln:
	rm -rf ${OUTPUT}
