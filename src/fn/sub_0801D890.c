#include "global.h"
#define ResultTutorial_DemonstrateLatePress sub_0801D890
extern int gUnk_0824DB94[];
extern u8 gUnk_0824DB2D[];
void sub_0801D798(void *, int);
#define I(o) (*(int *)(p + (o)))
int ResultTutorial_DemonstrateLatePress(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            p[0xb0] = 255;
            p[0xb1] = 0;
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            I(0x8c) += 1024;
            p[0xb0] = 255;
            if (I(0x8c) > 5631) {
                I(0x8c) = 5632;
                p[0xb0] = 0;
                ++p[0xaf];
            }
            break;
        case 2: {
            int width;
            I(0x8c) -= gUnk_0824DB94[4];
            width = gUnk_0824DB2D[p[0xae]] << 5;
            if (I(0x8c) - I(0x9c) < width - 256)
                sub_0801D798(p, 1);
            if (I(0x8c) <= 0) {
                I(0x8c) = 0;
                return 1;
            }
            break;
        }
    }
    return 0;
}
