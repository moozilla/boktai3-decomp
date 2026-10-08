#include "global.h"

s32 sub_08013C74(u8 *head, u8 *n) {
    u8 *prev = *(u8 **)(n + 0x4C);
    u8 *next = *(u8 **)(n + 0x50);
    s32 r;
    if (n[2] == 0) {
        return -1;
    }
    if (prev) {
        *(u8 **)(prev + 0x50) = next;
    } else {
        *(u8 **)(head + 0x18) = next;
    }
    if (next) {
        *(u8 **)(next + 0x4C) = prev;
    }
    r = 0;
    n[2] = r;
    return r;
}
