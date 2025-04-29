# XV6

This repository contains individual solutions to the assignment labs of <i>6.1810: Operating System Engineering</i> class in Standford University. All the resources could be also found in [6.1810 Fall 2024](https://pdos.csail.mit.edu/6.828/2024/index.html)

## Setup

For this repository we'll need RISC-V versions of QEMU, multiarch GDB, GCC, and Binutils to build xv6.  

Following steps are run and tested in Ubuntu 24.04. Other platforms could refer to [Tools](https://pdos.csail.mit.edu/6.828/2024/tools.html).

Steps:

* Clone this repository

  ```
  git clone https://github.com/shinnenijou/xv6-labs-2024
  ```

* Install build tools

  ```
  sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
  ```

* Cd into the repository then run make command, then the kernel will automatically run after successful  building

  ```
  cd xv6-labs-2024
  make qemu
  ```

* In some branches, you could make grade target to run auto grader to test the solution.

  ```
  make grade
  ```

  

## Grading

This section summarizes the auto grading results to each lab. Some prints by the build tools and commands by the auto grader are omitted.

### Lab Utilities

Branch <i>Util</i> contains the solution to [Lab Utilities](https://pdos.csail.mit.edu/6.828/2024/labs/util.html).

```
$ git checkout util
...
$ make clean
...
$ make grade
== Test sleep, no arguments ==
sleep, no arguments: OK (3.2s)
== Test sleep, returns ==
sleep, returns: OK (1.0s)
== Test sleep, makes syscall ==
sleep, makes syscall: OK (1.0s)
== Test pingpong ==
pingpong: OK (1.2s)
== Test primes ==
primes: OK (1.6s)
== Test find, in current directory ==
find, in current directory: OK (0.8s)
== Test find, in sub-directory ==
find, in sub-directory: OK (0.8s)
== Test find, recursive ==
find, recursive: OK (1.7s)
== Test xargs ==
xargs: OK (2.2s)
== Test xargs, multi-line echo ==
xargs, multi-line echo: OK (0.7s)
== Test time ==
time: OK
Score: 110/110
```

### Lab System calls

Branch <i>syscall</i> contains the solution to [Lab System calls](https://pdos.csail.mit.edu/6.828/2024/labs/syscall.html).

``` 
$ git checkout syscall
...
$ make clean
...
$ make grade
== Test answers-syscall.txt ==
answers-syscall.txt: OK
== Test trace 32 grep ==
trace 32 grep: OK (3.0s)
== Test trace close grep ==
trace close grep: OK (0.2s)
== Test trace exec + open grep ==
trace exec + open grep: OK (1.0s)
== Test trace all grep ==
trace all grep: OK (1.0s)
== Test trace nothing ==
trace nothing: OK (0.9s)
== Test trace children ==
trace children: OK (19.4s)
== Test attack ==
attack: OK (0.2s)
== Test time ==
time: OK
Score: 50/50
```

### Lab Page tables

Branch <i>pgtbl</i> contains the solution to [Lab Page tables](https://pdos.csail.mit.edu/6.828/2024/labs/pgtbl.html).

```
$ git checkout syscall
...
$ make clean
...
$ make grade
== Test pgtbltest ==
(3.7s)
== Test   pgtbltest: ugetpid ==
  pgtbltest: ugetpid: OK
== Test   pgtbltest: print_kpgtbl ==
  pgtbltest: print_kpgtbl: OK
== Test   pgtbltest: superpg ==
  pgtbltest: superpg: OK
== Test answers-pgtbl.txt ==
answers-pgtbl.txt: OK
== Test usertests ==
(164.4s)
== Test   usertests: all tests ==
  usertests: all tests: OK
== Test time ==
time: OK
Score: 51/51
```

### Lab Traps

Branch <i>traps</i> contains the solution to [Lab Page tables](https://pdos.csail.mit.edu/6.828/2024/labs/pgtbl.html).

```
$ git checkout traps
...
$ make clean
...
$ make grade
== Test answers-traps.txt == 
answers-traps.txt: OK 
== Test backtrace test == 
backtrace test: OK (3.1s) 
== Test running alarmtest == 
(4.2s) 
== Test   alarmtest: test0 == 
  alarmtest: test0: OK 
== Test   alarmtest: test1 == 
  alarmtest: test1: OK 
== Test   alarmtest: test2 == 
  alarmtest: test2: OK 
== Test   alarmtest: test3 == 
  alarmtest: test3: OK 
== Test usertests == 
usertests: OK (182.0s) 
== Test time == 
time: OK 
Score: 95/95
```

### Lab Copy-on-Write

Branch <i>cow</i> contains the solution to [Lab Copy-on-Write](https://pdos.csail.mit.edu/6.828/2024/labs/cow.html).

```
$ git checkout cow
...
$ make clean
...
$ make grade
== Test running cowtest == 
(30.7s) 
== Test   simple == 
  simple: OK 
== Test   three == 
  three: OK 
== Test   file == 
  file: OK 
== Test   forkfork == 
  forkfork: OK 
== Test usertests == 
(185.1s) 
== Test   usertests: copyin == 
  usertests: copyin: OK 
== Test   usertests: copyout == 
  usertests: copyout: OK 
== Test   usertests: all tests == 
  usertests: all tests: OK 
== Test time == 
time: OK 
Score: 130/130
```

### Lab Networking

Branch <i>net</i> contains the solution to [Lab Networking](https://pdos.csail.mit.edu/6.828/2024/labs/net.html).

```
$ git checkout net
...
$ make clean
...
$ make grade
== Test   nettest: txone == 
  nettest: txone: OK 
== Test   nettest: arp_rx == 
  nettest: arp_rx: OK 
== Test   nettest: ip_rx == 
  nettest: ip_rx: OK 
== Test   nettest: ping0 == 
  nettest: ping0: OK 
== Test   nettest: ping1 == 
  nettest: ping1: OK 
== Test   nettest: ping2 == 
  nettest: ping2: OK 
== Test   nettest: ping3 == 
  nettest: ping3: OK 
== Test   nettest: dns == 
  nettest: dns: OK 
== Test   nettest: free == 
  nettest: free: OK 
== Test time == 
time: OK 
Score: 171/171
```

### Lab Locks

Branch <i>lock</i> contains the solution to [Lab Locks](https://pdos.csail.mit.edu/6.828/2024/labs/lock.html).

```
$ git checkout lock
...
$ make clean
...
$ make grade
== Test running kalloctest == 
(116.1s) 
== Test   kalloctest: test1 == 
  kalloctest: test1: OK 
== Test   kalloctest: test2 == 
  kalloctest: test2: OK 
== Test   kalloctest: test3 == 
  kalloctest: test3: OK 
== Test kalloctest: sbrkmuch == 
kalloctest: sbrkmuch: OK (16.6s) 
== Test running bcachetest == 
(186.0s) 
== Test   bcachetest: test0 == 
  bcachetest: test0: OK 
== Test   bcachetest: test1 == 
  bcachetest: test1: OK 
== Test   bcachetest: test2 == 
  bcachetest: test2: OK 
== Test   bcachetest: test3 == 
  bcachetest: test3: OK 
== Test usertests == 
usertests: OK (192.6s) 
== Test time == 
time: OK 
Score: 110/110
```

### Lab File System

Branch <i>fs</i> contains the solution to [Lab File System](https://pdos.csail.mit.edu/6.828/2024/labs/fs.html).

```
$ git checkout fs
...
$ make clean
...
$ make grade
== Test running bigfile == 
running bigfile: OK (178.2s) 
== Test running symlinktest == 
(1.9s) 
== Test   symlinktest: symlinks == 
  symlinktest: symlinks: OK 
== Test   symlinktest: concurrent symlinks == 
  symlinktest: concurrent symlinks: OK 
== Test usertests == 
usertests: OK (265.1s) 
== Test time == 
time: OK 
Score: 100/100
```

### Lab mmap

Branch <i>mmap</i> contains the solution to [Lab mmap](https://pdos.csail.mit.edu/6.828/2024/labs/mmap.html).

```
$ git checkout mmap
...
$ make clean
...
$ make grade
== Test running mmaptest == 
(8.2s) 
== Test   mmaptest: mmap basic == 
  mmaptest: mmap basic: OK 
== Test   mmaptest: mmap private == 
  mmaptest: mmap private: OK 
== Test   mmaptest: mmap read-only == 
  mmaptest: mmap read-only: OK 
== Test   mmaptest: mmap read/write == 
  mmaptest: mmap read/write: OK 
== Test   mmaptest: mmap dirty == 
  mmaptest: mmap dirty: OK 
== Test   mmaptest: not-mapped unmap == 
  mmaptest: not-mapped unmap: OK 
== Test   mmaptest: lazy access == 
  mmaptest: lazy access: OK 
== Test   mmaptest: two files == 
  mmaptest: two files: OK 
== Test   mmaptest: fork_test == 
  mmaptest: fork_test: OK 
== Test   mmaptest: munmap_noaccess == 
  mmaptest: munmap_noaccess: OK 
== Test   mmaptest: read_only_write == 
  mmaptest: read_only_write: OK 
== Test usertests == 
usertests: OK (195.0s) 
== Test time == 
time: OK 
Score: 170/170
```

