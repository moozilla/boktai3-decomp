#include "global.h"
struct VecB278 { u32 x:16, y:16, z:16; };
void sub_0821FEB4(u8 *, s32, s32, s32, s32, struct VecB278 *, struct VecB278 *);
void sub_0821FF58(u8 *, s32, s32, s32, s32, s32);
void sub_0821FF84(u8 *, void *, u8 *);
void sub_0821FF24(u8 *, u8 *, s32);
void sub_082433C8(void);
void sub_0821FE40(u8 *);
void sub_0823B278(u8 *p)
{
    struct VecB278 scale;
    struct VecB278 position;
    u8 *q = p + 0x230;
    scale.x = 50;
    scale.y = 127;
    scale.z = 50;
    position.x = 0;
    position.y = 127;
    position.z = 0;
    sub_0821FEB4(q, *(u16 *)(p + 0x24), 0x4001, 0, (u16)(1U << *(u32 *)(p + 0x18)), &scale, &position);
    sub_0821FF84(q, sub_082433C8, p);
    sub_0821FF24(q, p + 0x168, 0);
    sub_0821FE40(q);
}
