#include "global.h"
void sub_0801C17C(void *);
void sub_0801C1C4(void *);
int ResultTutorial_InitializePressDemo(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            sub_0801C17C(p);
            sub_0801C1C4(p);
            p[0xb0] = 255;
            p[0xb1] = 0;
            *(int *)(p + 0x8c) = 0;
            *(int *)(p + 0x9c) = 1536;
            ++p[0xaf];
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            *(int *)(p + 0x8c) += 1024;
            if (*(int *)(p + 0x8c) > 3584) {
                *(int *)(p + 0x8c) = 3584;
                return 1;
            }
            break;
    }
    return 0;
}
