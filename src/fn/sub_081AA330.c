// MATCHING_CONCESSION: register-pinning
#include "vanargand_pair_attack.h"
#include "engine.h"
#include "ram.h"

struct T {
    u32 v[2];
};
extern const struct T gUnk_0824F790;
static inline s32 rnd(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}
void Vanargand_SetPendingCue(void *, u16);
u32 PairedSceneActor_GetActiveMask(struct VanargandAttackPair *);

void sub_081AA330(struct VanargandPairAttack *p)
{
    u32 m = 4;
    p->flags |= m;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending != 0) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    {
        u8 st = p->phase;
        switch (st) {
            case 0:
                if (p->timer == 0) {
                    struct T t;
                    switch ((s32)PairedSceneActor_GetActiveMask(p->pair)) {
                        case 0:
                            t = gUnk_0824F790;
                            PairedSceneActor_ApplyToSelectedHalves(p->pair, t.v[(rnd() >> 4) & 1],
                                                                   1);
                            break;
                        case 1:
                            PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 1);
                            break;
                        case 2:
                            PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 1);
                            break;
                    }
                    p->timer++;
                } else if (({
                               register u32 off asm("r2") = 0x56c;
                               register u8 *q asm("r0");
                               asm("" : "+r"(off));
                               q = (u8 *)p + off;
                               PairedSceneActor_AreSelectedHalvesFinished(*(struct VanargandAttackPair **)q);
                           }) != 0) {
                    p->phase = 1;
                    p->timer = 0;
                }
                break;
            case 1:
                p->finished = 1;
                break;
        }
    }
}
