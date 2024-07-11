SRCS=sv.c svsched.c syscalls.c
OBJ_DIR=../../obj/sv
OBJS=$(SRCS:%.c=$(OBJ_DIR)/%.o)

INCLUDES=-I. -I ../ -I ../util

.SUFFIXES: .c .o

all: dirs $(OBJS)

clean:
	@rm -f ${OBJS} svtest

test:
	gcc -o svtest -I . -I .. -D SV_UNIT_TEST -Wno-pointer-to-int-cast svsched.c

dirs:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c
	${CC} -o $@ -c ${CCFLAGS} ${INCLUDES} $<
