#include "global.h"
struct Vec5B { u32 x:16, y:16, z:16; };
extern u8 *gUnk_02000710;
void sub_0821FEB4(u8 *, s32, s32, s32, s32, struct Vec5B *, struct Vec5B *);
void sub_0821FF58(u8 *, s32, s32, s32, s32, s32);
void sub_0821FF84(u8 *, void *, u8 *);
void sub_0821FF24(u8 *, u8 *, s32);
void sub_08235B34(u8 *p)
{
    struct Vec5B scale, position;
    u8 *e = p + 0x94;
    s32 amount;
    u32 flags, step;
    scale.x = 30000; scale.y = 30000; scale.z = 30000;
    position.x = 0; position.y = 0; position.z = 0;
    if (*(u16 *)(p + 0x34c) == 0) {
        u8 *parent = *(u8 **)(p + 0x18);
        flags = 1U << parent[0x2c];
        amount = *(u16 *)(parent + 0x426) - *(u16 *)(parent + 0x424);
    } else {
        s32 value;
        if ((*(u8 **)(p + 0x18))[0x418] == 6) {
            value = *(s16 *)(gUnk_02000710 + 0x40) + 10;
            if (value > 99) value = 99;
        } else value = *(s16 *)(gUnk_02000710 + 0x40);
        flags = 0;
        amount = value * 8;
    }
    step = 0x1000;
    sub_0821FEB4(e, 0, 0x3001, 0, (u16)flags, &scale, &position);
    sub_0821FF58(e, amount, 10, 0, step, 30);
    sub_0821FF84(e, 0, p);
    sub_0821FF24(e, *(u8 **)(p + 0x18) + 0x30, 0);
}
