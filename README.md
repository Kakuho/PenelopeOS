# PenelopeOS

PenelopeOS is another pitiful attempt at building an operating system, targeting x86_64 processors. 

The operating system is currently cloned from my Chisaka, however this operating system follows the designs of Xinu,
and as such is much more simpler. I aim to use this to build foundational skills as Xinu does not implement many
advanced features such as memory paging / swapping. Chisaka shall be a grander project :D

## Building and Running

To compile the operating system to an .iso and run, type:
 
	make run

To run gdb for debugging purposes, type:

	make run-gdb

To run with qemu interrupt information, type:

	make run-debug.

More details can be found in the GNUMakefiles.
