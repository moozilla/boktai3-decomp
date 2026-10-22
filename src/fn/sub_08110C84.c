#include "global.h"

void sub_08110C84(void)
{
    s32 i = 0;
    do {
        s32 next = i + 1;
        s32 j = 31;
        do {
            j--;
        } while (j >= 0);
        i = next;
    } while (i <= 0);
}
