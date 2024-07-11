TEST_DIR=test
SRC_DIR=src

.PHONY: all test clean

all:
	make -C ${SRC_DIR} all

run:
	make -C ${SRC_DIR} run

clean:
	make -C ${SRC_DIR} clean
	make -C ${TEST_DIR} clean

test:
	make -C ${TEST_DIR} test
