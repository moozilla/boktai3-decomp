#include "global.h"
struct Vec339 { u32 x:16, y:16, z:16; };
void sub_0821FEB4(u8 *, s32, s32, s32, s32, struct Vec339 *, struct Vec339 *);
void sub_0821FF58(u8 *, s32, s32, s32, s32, s32);
void sub_0821FF84(u8 *, void *, u8 *);
void sub_0821FF24(u8 *, u8 *, s32);
void sub_08233924(u8 *p, u32 id, s32 a, s32 b)
{
    struct Vec339 scale;
    struct Vec339 position;
    u8 *q = p + 0x20;
    scale.x = 100;
    scale.y = 100;
    scale.z = 100;
    position.x = 0;
    position.y = 0;
    position.z = 0;
    sub_0821FEB4(q, 0, 0x2100, 0, (u16)id, &scale, &position);
    sub_0821FF58(q, a, 0, 0x20000, 0, 0);
    p[0x62] = b;
    sub_0821FF84(q, 0, p);
    sub_0821FF24(q, p + 0x8c, 0);
}
