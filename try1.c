/* This program is not intended to do much that is interesting, except
 * be used to generate some output from the Linux 'perf' tool.
 *
 * The call tree is:
 *
 * main
 *   bar1
 *     foo1
 *   bar2
 *     foo1
 *     foo2
 *
 * It is intentional that we call bar1 more often than bar2, so that
 * we can demonstrate in the 'perf report' output that it shows not
 * only the total time that is spent in foo1, but also that more time
 * is spent in foo1 when called from bar1, than when called from bar2.
 *
 * The program starts by printing a prompt and waiting for a single
 * line of input before doing the computation, so that the user has
 * time to determine the process ID of the program and start 'perf
 * record' after it has begun, but before it starts using significant
 * CPU time.
 */

#include <stdio.h>
#include <stdlib.h>


int foo1 (int n)
{
    int i;
    int sum = 0;

    for (i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

int foo2 (int n)
{
    int i;
    int sum = 0;

    for (i = 0; i < n; i++) {
        if ((i % 2) == 0) {
            sum += i;
        } else {
            sum -= i;
        }
    }
    return sum;
}


int bar1 (int n)
{
    int i;
    int sum = 0;

    for (i = 0; i < n; i++) {
        sum += foo1(i);
    }
    return sum;
}


int bar2 (int n)
{
    int i;
    int prod = 1;
    int sum = 0;

    for (i = 0; i < n; i++) {
        int tmp = foo1(i);
        if ((tmp != 0) && (prod * tmp) != 0) {
            prod *= tmp;
            sum += foo2(i);
        }
    }
    return (prod + sum);
}


int
main (int argc, char *argv[])
{
    int n;
    char buf[80];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <n>\n", argv[0]);
        exit(1);
    }
    n = atoi(argv[1]);

    printf("Press return to begin:\n");
    fgets(buf, sizeof(buf), stdin);

    printf("1:  bar1(%d)=%d\n", n, bar1(n));
    printf("2:  bar1(%d)=%d\n", n, bar1(n));
    printf("2b: bar2(%d)=%d\n", n, bar2(n));
    printf("3:  bar1(%d)=%d\n", n, bar1(n));
    printf("4:  bar1(%d)=%d\n", n, bar1(n));
    printf("5:  bar1(%d)=%d\n", n, bar1(n));
}
