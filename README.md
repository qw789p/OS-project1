# OS Project1

## kernel version
linux 4.14.25

## Description

Design a user-space scheduler based on the priority-driven scheduler built in Linux kernel for a set of child processes.
* scheduler.c : main process
* utils.c : used functions

## System Call Installation

1. Copy `kernel_file/my_time.c` to `linux/kernel/` directory
2. Add "`obj-y += my_time.o`" in `linux/kernel/Makefile`
3. Add
    ```
    asmlinkage int sys_my_time(int isStart, unsigned long *start_sec,
                                unsigned long *start_nsec, unsigned long *end_sec,
                                unsigned long *end_nsec, int *pid);
    ```
    in "`linux/include/linux/syscalls.h`"
4. Add `334 common my_time sys_my_time` in `linux/arch/x86/entry/syscalls/syscall_64.tbl`
5. Recompile and Reinstall
```sh
make bzImage -j4
sudo make install -j4
```
6. reboot

## Usage

```sh
cd src
make
sudo ./mysched < FILE.txt
sudo dmesg -c
```

## Sample output
```sh
P1 2603
P2 2604
P5 2607
P4 2606
P3 2605
```
```sh
[  774.114583] [project1] 2603 1587974687.274122987 1587974692.384189956
[  774.115962] [project1] 2604 1587974687.274190789 1587974692.385587977
[  774.152596] [project1] 2607 1587974687.274357233 1587974692.422223017
[  774.169665] [project1] 2606 1587974687.274304364 1587974692.439292078
[  774.185510] [project1] 2605 1587974687.274250670 1587974692.455138230
```
