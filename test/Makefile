SRCS=main.c munit/munit.c
OBJS=main.o munit/munit.o
EXEC=radon-tests

.SUFFIXES: .c .o

all: test

test: ${EXEC}
	./${EXEC}

clean:
	@rm -f ${EXEC} ${OBJS}

${EXEC}: ${OBJS}
	${CC} -o $@ ${LNFLAGS} ${LIBS} ${OBJS}

%.o: %.c
	${CC} -o $@ -c ${CCFLAGS} ${INCLUDES} $<
