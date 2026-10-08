#include "global.h"

void sub_082314C4(u8 *);

void sub_0807E7B0(u8 *p, s32 n) {
    if (p[0x47c] == 1 && n > 0) {
        s32 i = n;
        do {
            sub_082314C4(p + 0x47c);
        } while (--i != 0);
    }
}
