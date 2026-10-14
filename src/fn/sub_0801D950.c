#include "global.h"
extern int gUnk_0824DB94[];
extern u8 gUnk_0824DB2D[];
void ResultTutorial_Press(void *, int);
#define I(o) (*(int *)(p + (o)))
int ResultTutorial_DemonstrateAccuratePress(u8 *p)
{
    int state = p[0xaf];
    int *alias;
    switch (state) {
        case 0:
            p[0xaf] = 1;
            p[0xb0] = 255;
            p[0xb1] = state;
            *((u16 *)(p + 0xaa)) = state;
            break;

        case 1:
            *((int *)(p + 0x8c)) += 1024;
            p[0xb0] = 255;
            if ((*((int *)(p + 0x8c))) > 5631) {
                *((int *)(p + 0x8c)) = 5632;
                p[0xb0] = 0;
                ++p[0xaf];
            }
            break;

        case 2:
            alias = (int *)(p + 0x8c);
            *alias -= gUnk_0824DB94[4];
            state = *((int *)(p + 0x8c));
            if ((state - (*((int *)(p + 0x9c)))) <= 95) {
                ResultTutorial_Press(p, 1);
            }
            if ((*((int *)(p + 0x8c))) <= 0) {
                *((int *)(p + 0x8c)) = 0;
                return 1;
            }
            break;
    }

    return 0;
}
