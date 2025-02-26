CC = gcc
OUTPUT = main
SOURCE = main.c

all:
	${CC} ${SOURCE} -o ${OUTPUT}

clean:
	rm -rf ${OUTPUT}
