// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern int gUnk_0824DB38[];
extern u8 gUnk_0824DB64[], gUnk_0824DB6F[];
int ResultTutorial_MovePlayer(u8 *p, int input)
{
    register int pressed asm("r3") = input;
    int *v;
    if (pressed) {
        int *initial = (int *)(p + 0x90);
        *initial = gUnk_0824DB38[4];
        v = initial;
    } else {
        int *initial = (int *)(p + 0x90);
        int value;
        value = *initial;
        v = initial;
        if (value > 0) {
            *v = pressed;
            *((u8 *)initial + 0x22) = pressed;
        } else if (p[0xb2] < gUnk_0824DB64[4]) {
            *v = pressed;
            ++p[0xb2];
        } else {
            *v = value - gUnk_0824DB6F[4];
            if (*v < -22)
                *v = -22;
        }
    }
    *(int *)(p + 0x8c) += *v;
}
