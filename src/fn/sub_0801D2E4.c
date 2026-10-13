#include "global.h"
#define ResultTutorial_BlinkTarget sub_0801D2E4
void sub_0822B2F8(int);
void sub_0801C17C(void *);
void sub_0801C190(void *);
int ResultTutorial_BlinkTarget(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1: {
            int frame = *(u16 *)(p + 0xaa) & 31;
            int visible;
            if (frame == 0)
                sub_0822B2F8(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                sub_0801C17C(p);
            else
                sub_0801C190(p);
            if ((*(s16 *)(p + 0xaa) >> 5) > 2)
                return 1;
            ++*(u16 *)(p + 0xaa);
            break;
        }
    }
    return 0;
}
