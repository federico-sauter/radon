####################################################################################################
##  The main targets are:
##    make all       - Build the kernel and create a bootable disk image
##    make kernel    - Build the kernel
##    make run       - Build the kernel, create a bootable disk image and start a VM with it
##    make clean     - Delete intermediate files
##    make distclean - Delete intermediate files and directories
####################################################################################################

####################################################################################################
##	distribution directories
####################################################################################################
OBJ_DIR=../obj
BIN_DIR=../bin
UTIL_DIR=../util
IMG_DIR=../img
IA32_DIR=arch/ia32
SV_DIR=sv
LIB_DIR=util

####################################################################################################
## 	make targets configuration
####################################################################################################
KERNEL_IMG=radon32
KERNEL=${BIN_DIR}/${KERNEL_IMG}
DISK_IMG=${IMG_DIR}/radon.img
SRCS=ccio.c cckeybrd.c shell.c demo.c
OBJS=$(SRCS:%.c=$(OBJ_DIR)/%.o)
ARCH=IA32

## ia32init.o must go first!
IA32_OBJS=../obj/arch/ia32/ia32init.o \
../obj/arch/ia32/ia32traps.o \
../obj/arch/ia32/ia32irqs.o \
../obj/arch/ia32/ia32idt.o \
../obj/arch/ia32/ia32video.o \
../obj/arch/ia32/ia32sysinit.o \
../obj/arch/ia32/ia32tasks.o \
../obj/arch/ia32/ia32syscall.o \
../obj/arch/ia32/ia32macros.o \
../obj/arch/ia32/svtraps.o


####################################################################################################
##	tools configuration
####################################################################################################
export CC=gcc
export CCFLAGS=-Wall -m32 -nostartfiles \
		-Werror \
		-fstrength-reduce -fomit-frame-pointer \
		-ffreestanding \
		-nostdinc -fno-builtin \
                -D $(ARCH)
INCLUDES=-I. -I util -I include
LD=ld
LDFLAGS=-T sv.ld -melf_i386

####################################################################################################
##	make rules -- do not modify form here
####################################################################################################
.SUFFIXES: .c .o

all: ${DISK_IMG}

kernel: ${KERNEL}

run: ${DISK_IMG}
	${UTIL_DIR}/startvm.py --disk-image ${DISK_IMG}

clean:
	@rm -f ${OBJS} ${KERNEL} ${GRUB_CONFIG}
	make -C $(IA32_DIR) clean
	make -C $(SV_DIR) clean
	make -C $(LIB_DIR) clean

distclean: clean
	rm -rf $(BIN_DIR) $(OBJ_DIR) ${IMG_DIR}

dirs:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) ${IMG_DIR}

${DISK_IMG}: ${KERNEL}
	${UTIL_DIR}/mkbootdisk.py --kernel-image ${KERNEL} --disk-image ${DISK_IMG}

${KERNEL}: dirs ${OBJS} tinysh
	ARCH=$(ARCH) make -C $(IA32_DIR) all
	ARCH=$(ARCH) make -C $(SV_DIR) all
	ARCH=$(ARCH) make -C $(LIB_DIR) all
	${LD} -o ${KERNEL} ${LDFLAGS}  ${IA32_OBJS} ${OBJS} $(OBJ_DIR)/$(LIB_DIR)/*.o $(OBJ_DIR)/$(SV_DIR)/*.o $(OBJ_DIR)/tinysh.o

tinysh: tinysh.c
	${CC} -o $(OBJ_DIR)/tinysh.o -c -m32 -nostartfiles \
		-fstrength-reduce -fomit-frame-pointer \
		-ffreestanding -Wno-pointer-sign -Wno-unused-variable \
		-nostdinc -fno-builtin ${INCLUDES} $<

$(OBJ_DIR)/%.o: %.c
	${CC} -o $@ -c ${CCFLAGS} ${INCLUDES} $<
