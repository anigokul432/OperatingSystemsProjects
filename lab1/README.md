## UID: 005944382
(IMPORTANT: Only replace the above numbers with your true UID, do not modify spacing and newlines, otherwise your tarfile might not be created correctly)

## Pipe Up

This program implements the pipe operator from shell where output of the left hand side of the pipe will be the input for the right hand side of the pipe.

## Building

To build the program, first create a file and name it Makefile. Then, input the following code into that file.
```
OBJS = pipe.o

CFLAGS = -std=c17 -Wpedantic -Wall -O2 -pipe -fno-plt
LDFLAGS = -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
FILENAME=README.md
UID = $(shell cat ${FILENAME} | grep '## UID'| grep -oe '\([0-9.]*\)')

pipe: ${OBJS}

tar:
	@tar -cf ${UID}-lab1-submission.tar pipe.c README.md
.PHONY: clean
clean:
	@rm -f ${OBJS} pipe
```
Now, run the command `make` in the terminal.

## Running

To run the program, just type the following command after running `make`.
```
./pipe x y z
```
where x, y and z are shell commands. Make sure there are no attributes to the shell command. For example, ls -l would not work.

An example of running pipe would be the following:
```
cs111@cs111 CS111/lab1 » ./pipe ls cat wc
      7       7      63
```

## Cleaning up

Since you already have the Makefile set up, simply run the following command to clean up all binary files.
```
make clean
```
