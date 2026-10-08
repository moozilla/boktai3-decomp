#include "global.h"

u32 sub_0800CC08(u8 *head, u8 *n) {
    u8 *prev = *(u8 **)(n + 0xEC);
    u8 *next = *(u8 **)(n + 0xF0);
    if (prev) {
        *(u8 **)(prev + 0xF0) = next;
    } else {
        *(u8 **)(head + 0x1C) = next;
    }
    if (next) {
        *(u8 **)(next + 0xEC) = prev;
    }
    return 0;
}
