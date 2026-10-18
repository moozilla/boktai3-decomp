#include "global.h"

typedef struct {
    u8 pad0[0x2c];
    u8 obj[6];
    u16 f32;
    u8 pad1[4];
    void *f38;
    u8 pad2[0xf6 - 0x3c];
    u16 idx;
    u8 pad3[8];
    u8 cnt;
    u8 pad4[0x114 - 0x101];
    void *arg;
} Ent;

typedef struct {
    u8 pad0[0x18];
    u8 n;
    u8 pad1[0x5c - 0x19];
    Ent e[2];
    u8 pad2[0x4bc - 0x5c - 2 * 0x118];
    u8 *q;
} Obj;

extern void sub_081B7B1C(Obj *p);
extern s32 sub_0824923C(void *arg, void *function);
extern void sub_08215284(void *a, s32 b);
extern u32 D_08612154[];

s32 sub_081B7C98(Obj *p)
{
    s32 i;
    sub_081B7B1C(p);
    if (*(s16 *)(p->q + 0x94) > 0) {
        for (i = 0; i < p->n; i++) {
            Ent *e = &p->e[i];
            sub_0824923C(e, e->arg);
            if (e->cnt != 0) {
                e->cnt--;
                if (e->cnt != 0) {
                    e->f32 = D_08612154[e->idx];
                    e->f38 = (u8 *)p + (e->idx * 32 + 0x1c);
                } else {
                    sub_08215284(&e->obj, 0xd5);
                }
            }
        }
    }
    return 0;
}
