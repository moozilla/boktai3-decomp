#include "global.h"

s32 sub_08049840(u8 *, s32, s32);

s32 sub_08156FC0(u8 *p)
{
    return sub_08049840(p + 0x30, *(u16 *)(p + 0x476), *(u16 *)(p + 0x474));
}
