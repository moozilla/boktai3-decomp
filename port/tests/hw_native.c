#include <stdio.h>
int hw_test_run(void);
int main(void)
{
    int line = hw_test_run();
    if (line) { fprintf(stderr, "Hardware test failed at hw_test.c:%d\n", line); return 1; }
    puts("Hardware tests: PASS (native)");
    return 0;
}
