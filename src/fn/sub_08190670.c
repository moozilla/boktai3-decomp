#include "global.h"
struct P {
    u8 f0[0x4c];
    u32 w;
    u8 f1[0x5DFC - 0x50];
    u8 q[0x724D - 0x5DFC];
    u8 b;
};
extern const u8 gUnk_0824F2BC[];
extern const u8 gUnk_0824F2C4[];
void Collider_Init(void *, u32, s32, s32, s32, const void *, const void *);
void sub_0821FF58(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, s32, s32);
void Sound_PlaySongChecked(s32);
void sub_08190670(struct P *p)
{
    u32 k;
    u8 *b = &p->b;
    *b = k = 0x1e;
    Collider_Init(p->q, (u16)p->w, 0x2001, 0, 0x10, gUnk_0824F2BC, gUnk_0824F2C4);
    sub_0821FF58(p->q, 0x7FFF, 200, 0, 0x40000, k);
    Collider_SetCallback(p->q, 0, 0);
    Sound_PlaySongChecked(0x1bc);
}
