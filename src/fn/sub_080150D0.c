#include "global.h"
#include "ram.h"
int FragmentManager_Initialize(u8 *m)
{
    int i = 0;
    do {
        u8 *g = m + 32 + i * 376;
        int next;
        int j;
        g[0] = 0;
        next = i + 1;
        for (j = 5; j >= 0; j--)
            g[44 + j] = 0;
        i = next;
    } while (i <= 3);
    *(u32 *)(m + 24) = 0;
    gFragmentManager = m;
    return 0;
}
