The Radon project aims to create a microkernel for multicore embedded systems.

# Building and Running

## Prerequisites

The build system has been tested on Ubuntu 23.04 (Lunar). Install the following packages:
```
sudo apt-get install build-essential nasm grub-pc-bin qemu-kvm virt-manager virt-viewer
```

## Build

To build and run in a VM:
```
make run
```

To stop the VM, kill the VM process manually or execute `util/stopvms.sh`, which kills all running VMs.

## Other Make Targets

* `make all`: Build the kernel and create a bootable disk image.
* `make kernel`: Build the kernel binary.
* `make run`: Build the kernel, create a bootable disk image, and start a VM.
* `make clean`: Delete intermediate files.
* `make distclean`: Delete intermediate files and directories.

# Implementation Notes

Developing an operating system kernel involves tightly interdependent components, making gradual implementation challenging.

The primary goal is to implement a multitasking pre-emptive kernel with interactive capabilities. Non-essential work, such as memory management and file systems, is deferred.

## Project Organization

- `bin`: Kernel binary.
- `img`: Bootable operating system image.
- `util`: Utility programs to expedite testing.

### Kernel Sources

- `include`: Header files defining the kernel API for user-space programs.
- `sv`: Platform-independent supervisor (kernel main program) code.
- `sys`: System headers relevant to the kernel.
- `util`: Standard library and utility functions for the supervisor.

### IA-32 Architecture-Specific Portions

Architecture-specific code resides in `src/arch/ia32/`. Task state, defined as IA-32 registers, is the only exception to architecture independence.

Minimal assembly code is used to simplify kernel understanding.

### Naming Conventions

Naming conventions aid discoverability:
```
<component>_<object>_<verb>_<thing>
```

**Component Acronyms**:
* `sv`: Supervisor
* `cc`: Console controller
* `ia32`: Intel 32-bit architecture-specific implementation

**Object**: The object within the component context.
**Verb**: The action performed by the function.
**Thing**: The target object of the function operation (optional).

## The Supervisor Program

The supervisor initializes the system, remaps programmable IRQs, handles faults, spawns new tasks, and manages task-related operations.

"Supervisor" and "kernel" are used interchangeably.

## The Shell Task

The shell task controls user interaction and currently handles keyboard interrupts directly, to be fixed in a subsequent release.

### Available Shell Commands

- `ver`: Shows the operating system version.
- `clear`: Clears the screen.
- `ps`: Prints the list of currently running tasks.
- `help`: Shows shell usage information.
- `exit-test [wait]`: Demonstrates task exit and optional wait syscall.
- `demo`: Multitasking demonstration program (requires system restart to exit).

## Task Switching

Task state is temporarily stored in global variables (`_eax`, `_ebx`, etc.) during system calls and IRQ handling. This intermediate storage is inefficient and should be optimized in future updates.

### Task Context

Task context comprises the CPU register state for the next instruction. Segment registers (`cs, ds, es, fs, gs, ss`) are not included as the system uses a flat memory model.

## Task Scheduler

The scheduler uses a task table with a 32-task limit, with 30 slots available for user tasks. A round-robin scheme is used, prioritizing the keyboard interrupt.

The current fixed-size data structures result from the lack of heap or memory management and will be reimplemented once these are available.

## IA-32 System Initialization and Data Structures

Data structures for IA-32 CPU initialization are defined in `arch/ia32/ds.h`:

- `gdt_elem_t`: Entry in the Global Descriptor Table (GDT).
- `idt_elem_t`: Entry in the Interrupt Descriptor Table (IDT).
- `load_ptr_t`: Pointer to the GDT or IDT.

The supervisor is loaded by a multiboot-compliant bootloader like GRUB2. Initial setup occurs in `ia32init.asm`, configuring the supervisor stack and jumping to the main routine.

Interrupt handling subroutines in `ia32traps.asm` save context, call handlers, and return using `IRET`. The `SAVE_CTX` and `RETR_CTX` macros in `ia32macros.asm` define the saved context, excluding segment registers due to the flat memory model.

# Limitations

- Requires BIOS to boot; UEFI not supported.
- No heap or memory management; all operations are stack-based.
- No user-space/kernel-space distinction; all tasks are kernel tasks.
- Console lacks function key support, including backspace.

# Copyright Notice

© 2024 Federico Sauter. All rights reserved.

Includes `tinysh` © 2001 Michel Gutierrez.
