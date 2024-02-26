## UID: 005944382
(IMPORTANT: Only replace the above numbers with your true UID, do not modify spacing and newlines, otherwise your tarfile might not be created correctly)

# Hash Hash Hash

This code implements hash tables that are safe to use concurrently, with the first implementation using a single mutex and the second implementation using multiple mutexes.

## Building

To build the program, create a file in the directory called Makefile and paste the following code inside the file.
```
ifeq ($(shell uname -s),Darwin)
	CFLAGS = -std=gnu17 -pthread -Wall -O0 -pipe -fno-plt -fPIC -I. -I/opt/homebrew/include
	LDFLAGS = -pthread -L$(shell brew --prefix)/lib -largp
else
	CFLAGS = -std=gnu17 -pthread -Wall -O0 -pipe -fno-plt -fPIC -I.
	LDFLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif


OBJS = \
  hash-table-common.o \
  hash-table-base.o \
  hash-table-v1.o \
  hash-table-v2.o \
  hash-table-tester.o

FILENAME=README.md
UID = $(shell cat ${FILENAME} | grep '## UID'| grep -oe '\([0-9.]*\)')
SUBMISSION_FILES = hash-table-v1.c hash-table-v2.c README.md
.PHONY: all
all: hash-table-tester

hash-table-tester: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
tar:
	@tar -cf ${UID}-lab3-submission.tar ${SUBMISSION_FILES}

.PHONY: clean
clean:
	rm -f $(OBJS) hash-table-tester

```
Then, you build the program by running `make` which will give you the hash-table-tester executable.

## Running

```
./hash-table-tester -t 8 -s 50000    
Generation: 91,860 usec
Hash table base: 1,180,327 usec
  - 0 missing
Hash table v1: 1,238,643 usec
  - 0 missing
Hash table v2: 794,667 usec
  - 0 missing
```

## First Implementation

In version 1, thread safety was achieved using a single mutex. The function `hash_table_v1_add_entry` is now an atomic function. There won't be any inconsistent states since only a single thread can modify the hash table each time. The mutex is created and destroyed along with the hash map. This is the most simple way to approach this. 

### Performance

Low Threads:
```
./hash-table-tester -t 4 -s 100000
Generation: 114,620 usec
Hash table base: 1,649,461 usec
  - 0 missing
Hash table v1: 1,774,638 usec
  - 0 missing
Hash table v2: 1,105,325 usec
  - 0 missing
```
High Threads:
```
./hash-table-tester -t 8 -s 50000
Generation: 109,458 usec
Hash table base: 1,551,176 usec
  - 0 missing
Hash table v1: 1,510,579 usec
  - 0 missing
Hash table v2: 956,556 usec
  - 0 missing
```

## Second Implementation

In version 2, I assigned a mutex to each bucket. This makes it so different threads can access the buckets concurrently. Each mutex is created when the hash table is created. When operated on, a single bucket's mutex gets locked. This makes all buckets atomic and prevents deadlocks. This is much more performance optimized for that reason.

### Performance

```
./hash-table-tester -t 4 -s 100000
Generation: 114,620 usec
Hash table base: 1,649,461 usec
  - 0 missing
Hash table v1: 1,774,638 usec
  - 0 missing
Hash table v2: 1,105,325 usec
  - 0 missing
```
Version 2 uses a finer-grained locking with a mutex for each hash table bucket, allowing multiple threads to concurrently operate on different buckets. In contrast, version 1 uses a single mutex for the entire table, causing threads to wait even for non-conflicting operations. This granularity in version 2 reduces contention, optimizes parallelism, and leads to enhanced performance on multi-core systems.

## Cleaning up

Run the command `make clean` to remove all binary files.
