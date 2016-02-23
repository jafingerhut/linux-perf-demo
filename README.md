# Purpose

This project is just a very simple demonstration of using the Linux
'perf' utility to measure where the time is spent while running a user
space application.

It consists of a short C program that can be compiled and run on a
Linux machine, some sample 'perf' commands, and sample output with
notes on interpreting what that output means.


# Interpreting the perf output

The sample output file `perf-report.txt` included in this repository
contains these lines:

    # Overhead  Command      Shared Object                  Symbol
    # ........  .......  .................  ......................
    #
        93.42%     try1  try1               [.] foo1              
                   |
                   --- foo1
                      |          
                      |--83.31%-- bar1
                      |          main
                      |          __libc_start_main
                      |          
                      |--16.68%-- bar2
                      |          main
                      |          __libc_start_main
                       --0.00%-- [...]
    
         6.54%     try1  try1               [.] foo2              
                   |
                   --- foo2
                       bar2
                       main
                       __libc_start_main

The line starting with 93.42% indicates that 93.42% of the time
samples were while process `try1` was in the function `foo1`.  Later
we see that 6.54% of the time samples were while process `try1` was in
the function `foo2`.

Of that 93.42% of the total process time in `foo1`, 83.31% of that
time was when `foo1` was being called from function `bar1`, which in
turns was called from `main`, which in turn was called from
`__libc_start_main`.  That last one is not in the source code of the
program, and I presume it is some kind of Linux C runtime thing that
is normally hidden from view.

Of the 93.42% of the total process time in `foo1`, 16.68% of that time
was when `foo1` was being called from function `bar2`, which in turn
was called from `main`, which in turn was called from
`__libc_start_main`.

Thus if you wanted to optimize this program, obviously function `foo1`
is where most of the total time is being spent, but of that, most of
it is when being called from `bar1`.  If you have a function like
`snprintf` that is called from many places in your program, this kind
of "reverse call tree" with percentages can help you determine where
it is being called from, and of those places it is called from, which
ones represent the most time.

Of the 6.54% of the total process time in `foo2`, all of it was when
`foo2` was called from function `bar2`, which in turn was called from
`main`, which in turn was called from `__libc_start_main`.


# Running it yourself

To compile:

    % make try1

To run, and record perf data:

    % ./try1 30000
    Press return to begin:

Before pressing return, in another shell window enter this command:

    % ps xguw|grep try1
    andy      11149  0.0  0.0   4196   352 pts/0    S+   23:07   0:00 ./try1 30000
    andy      11151  0.0  0.0  15936   944 pts/1    S+   23:08   0:00 grep try1

The second column of the line for the command `./try1 30000` is the
process ID of the `try1` process.  Start perf recording for it using
this command:

    % ./perf-record.sh 11149
    + perf record --call-graph fp --pid=11149
    WARNING: Kernel address maps (/proc/{kallsyms,modules}) are restricted,
    check /proc/sys/kernel/kptr_restrict.
    
    Samples in kernel functions may not be resolved if a suitable vmlinux
    file is not found in the buildid cache or in the vmlinux path.
    
    Samples in kernel modules won't be resolved at all.
    
    If some relocation was applied (e.g. kexec) symbols may be misresolved
    even with a suitable vmlinux or kallsyms file.

Now that it is recording, go back to the original shell window where
you ran `try1` and press return.  You should see output similar to
this:

    1:  bar1(30000)=-1575716208
    2:  bar1(30000)=-1575716208
    2b: bar2(30000)=2147483630
    3:  bar1(30000)=-1575716208
    4:  bar1(30000)=-1575716208
    5:  bar1(30000)=-1575716208

When it is done, go back to the window where the `perf-record.sh`
script is running, and terminate it by typing Control-C.  You should
see output similar to this:

    [ perf record: Woken up 10 times to write data ]
    [ perf record: Captured and wrote 2.432 MB perf.data (~106268 samples) ]
    [kernel.kallsyms] with build id 0b903d60e05414102a3dd797a0505a25a7e2607d not found, continuing without symbols

A file `perf.data` should now have been created.  You can generate a
text report from this file as follows:

    % ./perf-report.sh > perf-report.txt
