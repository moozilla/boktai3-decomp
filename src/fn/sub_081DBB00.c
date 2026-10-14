// COMPILER: old_agbcc
#include "global.h"

extern struct {
    u8 pad[0x20];
    u32 value;
} gUnk_020002C0;

u32 sub_081DBB00(void)
{
    return gUnk_020002C0.value;
}
