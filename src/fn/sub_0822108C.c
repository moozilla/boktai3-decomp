#include "global.h"
struct Range {
    u16 unk0;
    u16 count;
    u32 offset;
};
struct Item {
    u16 a;
    u16 v;
};
struct P {
    u8 pad[0x10];
    struct Range *ranges;
    u32 unk14;
    u8 *items;
};
s32 sub_0822108C(u32 unused, struct P *p, s32 index)
{
    s32 i, sum;
    struct Item *list;
    struct Range *table = p->ranges;
    struct Range *entry = (struct Range *)((index * 8) + (u32)table);
    list = (struct Item *)(p->items + entry->offset);
    for (i = 0, sum = 0; i < entry->count; i++)
        sum += list[i].v;
    return sum;
}
