// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"

// foundry draft (worker best 98.4%); what differs is mostly register allocation and instruction order
struct A {
    u8 pad0[0x9c];
    u32 flags;
    u8 pad1[2];
    u8 state;
    u8 pad2;
    u32 timer;
    u8 pad3[0xaf - 0xa8];
    u8 f_af;
    u8 pad4;
    u8 f_b1;
    u8 pad5[0x56c - 0xb2];
    void *obj;
};

struct T {
    u32 v[2];
};
extern const struct T gUnk_0824F790;
static inline s32 rnd(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}
void Vanargand_SetPendingCue(struct A *p, u16 v);
s32 PairedSceneActor_GetActiveMask(void *o);
void PairedSceneActor_ApplyToSelectedHalves(void *o, u32 a, u32 b);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *o);

void sub_081AA330(struct A *p)
{
    u32 m = 4;
    p->flags |= m;
    Vanargand_SetPendingCue(p, 1);
    if (p->f_b1 != 0) {
        p->f_b1 = 0;
        p->state = 0;
        p->timer = 0;
    }
    {
        u8 st = p->state;
        switch (st) {
            case 0:
                if (p->timer == 0) {
                    struct T t;
                    switch (PairedSceneActor_GetActiveMask(p->obj)) {
                        case 0:
                            t = gUnk_0824F790;
                            PairedSceneActor_ApplyToSelectedHalves(p->obj, t.v[(rnd() >> 4) & 1],
                                                                   1);
                            break;
                        case 1:
                            PairedSceneActor_ApplyToSelectedHalves(p->obj, 2, 1);
                            break;
                        case 2:
                            PairedSceneActor_ApplyToSelectedHalves(p->obj, 1, 1);
                            break;
                    }
                    p->timer++;
                } else if (({
                               register u32 off asm("r2") = 0x56c;
                               register u8 *q asm("r0");
                               asm("" : "+r"(off));
                               q = (u8 *)p + off;
                               PairedSceneActor_AreSelectedHalvesFinished(*(void **)q);
                           }) != 0) {
                    p->state = 1;
                    p->timer = 0;
                }
                break;
            case 1:
                p->f_af = 1;
                break;
        }
    }
}
