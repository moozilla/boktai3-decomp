#include "global.h"

void sub_08013B74(u8 *p);

void sub_0800CC34(u8 *p) {
    if (p[0xB]) {
        sub_08013B74(p + 0x80);
        p[0xB] = 0;
    }
}
