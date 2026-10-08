#include "global.h"

extern u32 *gUnk_020004AC;

u32 sub_08070EBC(u32 a) {
    u32 *n = gUnk_020004AC;
    u8 *e;
    while ((e = (u8 *)n[1]) != 0) {
        if (*(u32 *)(e + 0x50) == a) return (u32)e;
        n = (u32 *)*n;
    }
    return 0;
}
