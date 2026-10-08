#include "global.h"

s32 sub_080136DC(u8 *head, u8 *n) {
    u8 *prev = *(u8 **)(n + 0x64);
    u8 *next = *(u8 **)(n + 0x68);
    s32 r;
    if (n[0] == 0) {
        return -1;
    }
    if (prev) {
        *(u8 **)(prev + 0x68) = next;
    } else {
        *(u8 **)(head + 0x1C) = next;
    }
    if (next) {
        *(u8 **)(next + 0x64) = prev;
    }
    r = 0;
    n[0] = r;
    return r;
}
