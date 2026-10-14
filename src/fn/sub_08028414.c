#include "global.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0806A160(void *, int, void *, int, int, int, int, int);
void sub_08028414(u8 *p)
{
    struct Packed size;
    int angle;
    int speed;
    int count;
    u16 *table;
    u32 *indexPtr;
    u32 mask;
    u32 smallMask;
    u32 *alias;
    u32 index;
    size.x = 164;
    size.y = 16;
    size.z = 164;
    table = gUnk_0203B400;
    indexPtr = &gUnk_03005308;
    index = *indexPtr;
    index++;
    mask = 1023;
    index &= mask;
    angle = (u8)table[index];
    index = (index + 1) & mask;
    count = table[index];
    smallMask = 3;
    count = (count & smallMask) + 2;
    index = (index + 1) & mask;
    *indexPtr = index;
    alias = &index;
    speed = (table[*alias] & smallMask) + 20;
    sub_0806A160(p + 0x260, 3, &size, angle, count, speed, 2, 2);
    {
        u32 *ptr = indexPtr;
        index = ((*ptr) + 1) & mask;
    }
    angle = (u8)table[*alias];
    index = (index + 1) & mask;
    count = (table[index] & smallMask) + 2;
    index = (index + 1) & mask;
    *indexPtr = index;
    speed = (table[*alias] & smallMask) + 20;
    sub_0806A160(p + 0x260, 3, &size, angle, count, speed, 2, 2);
}
