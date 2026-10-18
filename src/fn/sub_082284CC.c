#include "global.h"
void Clock_WaitCycles(s32 n)
{
    if (n > 0) {
        do {
            n--;
        } while (n != 0);
    }
}
