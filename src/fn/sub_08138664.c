#include "global.h"
#include "engine.h"
#include "ram.h"
struct Q {
    u16 a, b;
    u8 pad[0x8c - 4];
    u16 x8c[4];
    u16 x94[4];
    u8 pad9c[0xf0 - 0x9c];
    u16 seed;
};
struct S {
    u8 pad0[0x30];
    u32 x30;
    u8 pad34[0x4e - 0x34];
    u8 x4e, x4f;
    u8 pad50[0x84 - 0x50];
    u16 x84[4];
    u8 pad8c[0x94 - 0x8c];
    u32 x94;
    u8 pad98[0xe0 - 0x98];
    struct Q q;
};
void Registry1745_InitParticipantOrder(struct S *p);
void sub_08138630(struct S *p, struct Q *q);

void sub_08138664(struct S *p)
{
    struct Q *q = &p->q;
    s32 i;
    q->b = q->a;
    p->x30 = 0;
    Registry1745_InitParticipantOrder(p);
    for (i = 0; i <= 3; i++) {
        q->x8c[i] = 0;
        q->x94[i] = 0;
        p->x84[i] = 0;
        gSceneSelectionDisplaySlots[i] = 0x3ff;
    }
    p->x94 = 0;
    p->x4e = 0;
    p->x4f = 0;
    sub_08138630(p, q);
    gRandomIndex = q->seed & 0x3ff;
}
