#include "global.h"
/* Anim_ApplyDurationScales: Scales the second animation's duration by both scale bytes */

s32 Div(s32, s32);
extern u8 *gGameState;

#define SCALE64(v) ((v) >= 0 ? (v) >> 6 : -(-(v) >> 6))

void Anim_ApplyDurationScales(u8 *a, u8 *b)
{
    s8 *pa = (s8 *)(a + 0x48);
    s8 *pb;
    if (*pa != 0x40) {
        if (*(s32 *)(a + 0x34) == 0x10) {
            s32 n = *(s16 *)(gGameState + 0x876);
            u16 *len = (u16 *)(b + 0x3e);
            *len += Div((*len << 1) * n, 100);
        } else {
            s32 v = *(u16 *)(b + 0x3e) * *pa;
            *(u16 *)(b + 0x3e) = SCALE64(v);
        }
    }
    pb = (s8 *)(b + 0x48);
    if (*pb != 0x40) {
        s32 v = *(u16 *)(b + 0x3e) * *pb;
        *(u16 *)(b + 0x3e) = SCALE64(v);
    }
}
