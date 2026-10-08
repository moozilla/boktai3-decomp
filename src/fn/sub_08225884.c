#include "global.h"
extern const u32 gUnk_08603300[];

u32 sub_08225844(u32, u32, u32, u32);

u32 sub_08225884(u32 a) {
    return sub_08225844(a, (u32)gUnk_08603300, 0, *(u16 *)0x030025F4);
}
