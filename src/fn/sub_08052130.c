#include "global.h"
struct S {
    u32 flags;
    u8 pad4[0x14];
    u16 x, y, z;
    u8 pad1e[0xa];
    u8 mode;
    u8 cnt;
    u8 pad2a[2];
    s16 vx, vy, vz;
};
u32 AnimData_FindEntry(u32);
void sub_08217EEC(void *, u32, u32);
static inline s32 dec(s32 v)
{
    s32 t = v * 15;
    if (t >= 0)
        return t >> 4;
    return -(-t >> 4);
}
void sub_08052130(struct S *s)
{
    u8 *pc = &s->cnt;
    u32 c;
    c = (*pc = *pc - 1);
    if (c == 0) {
        s->flags |= 1;
        s->mode = c;
    } else {
        s->x += s->vx;
        s->y += s->vy;
        s->z += s->vz;
        sub_08217EEC(s, AnimData_FindEntry(0x1C1E), ((*pc >> 2) & 1) + 2);
        s->vx = dec(s->vx);
        s->vy = dec(s->vy);
        s->vz = dec(s->vz);
    }
}
