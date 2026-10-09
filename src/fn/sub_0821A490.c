#include "global.h"
struct Header { u32 count; u32 keys, records, reserved; };
s32 sub_0821A454(u32, u16 *, s32, s32, s32);
u8 *sub_0821A490(u8 *base, u32 unused, u16 key, u16 context)
{
    struct Header header = *(struct Header *)base;
    s32 index;
    header.keys += (u32)base;
    header.records += (u32)base;
    index = sub_0821A454(key, (u16 *)header.keys, context, 0, header.count - 1);
    if (index < 0 || (u32)index >= header.count)
        return 0;
    return (u8 *)(((u32 *)header.records)[index] + (u32)base);
}
