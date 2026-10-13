// MATCHING_CONCESSION: register-pinning
#include "global.h"
int ResultTutorial_AnimateLevel(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            *(int *)(p + 0x50) = (*(s16 *)(p + 0xaa) >> 4) + 3;
            asm("" ::: "memory");
            if (++*(s16 *)(p + 0xaa) > 94) {
                ++p[0xaf];
                *(u16 *)(p + 0xaa) = 0;
            }
            break;
        case 2:
            *(int *)(p + 0x50) = 8 - (*(s16 *)(p + 0xaa) >> 4);
            asm("" ::: "memory");
            if (++*(s16 *)(p + 0xaa) > 78)
                return 1;
            break;
    }
    return 0;
}
