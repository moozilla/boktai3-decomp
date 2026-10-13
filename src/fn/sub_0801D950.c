// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern int gUnk_0824DB94[];
extern u8 gUnk_0824DB2D[];
void ResultTutorial_Press(void *, int);
#define I(o) (*(int *)(p + (o)))
int ResultTutorial_DemonstrateAccuratePress(u8 *p)
{
    int state = p[0xaf];
    asm("" : "+r"(state));
    switch (state) {
        case 0:
            p[0xaf] = 1;
            p[0xb0] = 255;
            p[0xb1] = state;
            *(u16 *)(p + 0xaa) = state;
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
        case 2:
            I(0x8c) -= gUnk_0824DB94[4];
            if (I(0x8c) - I(0x9c) <= 95)
                ResultTutorial_Press(p, 1);
            if (I(0x8c) <= 0) {
                I(0x8c) = 0;
                return 1;
            }
            break;
    }
    return 0;
}
