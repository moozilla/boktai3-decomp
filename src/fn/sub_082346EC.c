#include "global.h"
struct Vec346 { u32 x:16, y:16, z:16; };
void sub_0821FEB4(u8 *, s32, s32, s32, s32, struct Vec346 *, struct Vec346 *);
void sub_0821FF58(u8 *, s32, s32, s32, s32, s32);
void sub_0821FF84(u8 *, void *, u8 *);
void sub_0821FF24(u8 *, u8 *, s32);
void sub_082346EC(u8 *p)
{
    struct Vec346 scale;
    struct Vec346 position;
    u8 *q = p + 0x24;
    scale.x = 100;
    scale.y = 50;
    scale.z = 100;
    position.x = 0;
    position.y = 0;
    position.z = 0;
    sub_0821FEB4(q, 0, 0x2000, 0, 1, &scale, &position);
    sub_0821FF58(q, 0, 0, 0x40000, 0, 0);
    sub_0821FF84(q, 0, p);
    sub_0821FF24(q, p + 0x1c, 0);
}
