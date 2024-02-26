# OperatingSystemsProjects

Each lab contains each project I completed while learning Operating Systems Principles. All code (other than test-cases written in Python) are written using C.

## Lab 0
This kernel module creates a file /proc/count which displays the current number of running processes and deletes the file when the module is unloaded.

### Building

Type the following in the terminal to build the kernel module.
```
make
```

### Running

To run the kernel module, run the following command in the terminal.
```
sudo insmod proc_count.ko
```
To view the result, you can `cat /proc/file` to view the integer number of running processes.

### Cleaning Up

To remove the kernel module, enter the following code in the terminal.
```
sudo rmmod proc_count
```

### Testing

The kernel release version is `5.14.8-arch1-1`.

---

## Lab 1
This program implements the pipe operator from shell where output of the left hand side of the pipe will be the input for the right hand side of the pipe.

### Building

Run the command `make` in the terminal.

### Running

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

### Cleaning up

Since you already have the Makefile set up, simply run the following command to clean up all binary files.
```
make clean
```
---

## Lab 2
### Overview
This C program implements a Round Robin scheduling algorithm for process management. It reads process information from a file, schedules them based on a specified quantum time, and calculates the average waiting and response times for all processes.

### Features
- Utilizes the Round Robin scheduling technique for fairness among processes.
- Calculates and displays the average waiting and response times.
- Employs a queue to manage processes dynamically.

### Dependencies
- Standard C libraries: `errno.h`, `fcntl.h`, `stdbool.h`, `stdint.h`, `stdio.h`, `stdlib.h`, `unistd.h`
- System libraries: `sys/queue.h`, `sys/mman.h`, `sys/stat.h`

### Usage
Compile the program with a C compiler (e.g., `gcc`):

```sh
gcc rr.c -o rr
```

To Run:
```sh
./rr <path_to_process_file> <quantum_time>
```
---
## Lab 3
This code implements hash tables that are safe to use concurrently, with the first implementation using a single mutex and the second implementation using multiple mutexes.

### Building

Build the program by running `make` which will give you the hash-table-tester executable.

### Running

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

### First Implementation

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

### Second Implementation

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

### Cleaning up

Run the command `make clean` to remove all binary files.

---
## Lab 4

### Overview
This C program is designed to create a minimal EXT2 file system image, `cs111-base.img`. It sets up a basic EXT2 file system structure including the superblock, block group descriptor table, block and inode bitmaps, inode table, and directory entries for the root directory, lost+found directory, a regular file containing "Hello world\n", and a symbolic link named "hello" pointing to the "hello-world" file.

### Features
- Initializes a superblock with basic EXT2 file system parameters.
- Creates a single block group descriptor with initialized bitmaps for blocks and inodes.
- Populates the inode table with predefined inodes for the root directory, lost+found directory, a regular file, and a symbolic link.
- Writes directory entries for the root directory to include ".", "..", "lost+found", "hello-world", and "hello" links.
- Writes content "Hello world\n" into a file.

### Dependencies
The program requires standard C libraries such as `errno.h`, `fcntl.h`, `stdint.h`, `stdlib.h`, `stdio.h`, `string.h`, `sys/stat.h`, `time.h`, and `unistd.h`.

### Usage
Compile the program using a C compiler (e.g., `gcc`):
```sh
gcc ext2-create.c -o ext2-create
```

To Run:
```sh
./ext2-create
```
File System Structure
- Superblock: Contains overall information about the file system.
- Block Group Descriptor Table: Manages block groups, including the allocation status of blocks and inodes.
- Block Bitmap: Tracks the allocation status of blocks within the file system.
- Inode Bitmap: Tracks the allocation status of inodes.
- Inode Table: Contains inode information for files and directories.
- Root Directory Block: Contains entries for the root directory.
- Lost+Found Directory Block: Provides a directory for recovering lost files.
- Hello World File Block: Contains a sample file with the text "Hello world\n".
- Symbolic Link: A symbolic link named "hello" pointing to the "hello-world" file.
