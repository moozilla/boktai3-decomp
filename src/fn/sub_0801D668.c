#include "global.h"
void sub_0801C190(void *);
int ResultTutorial_SlideMarkerOut(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
            sub_0801C190(p);
            break;
        case 1:
            *(int *)(p + 0x8c) -= 176;
            if (*(int *)(p + 0x8c) <= 0) {
                *(int *)(p + 0x8c) = 0;
                return 1;
            }
            break;
    }
    return 0;
}
