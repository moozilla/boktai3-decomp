#include "global.h"

void sub_08223128(void);
void sub_08222CB0(void);

s32 sub_0822323C(void) {
    s32 r;
    sub_08223128();
    r = *(s32 *)0x03005324;
    if (r >= 0) {
        if (*(s32 *)0x0300531C != 0) {
            sub_08222CB0();
            r = 1;
        } else {
            sub_08222CB0();
            r = 0;
        }
    }
    return r;
}
