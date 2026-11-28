#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/engine.h"
#include "engine/ram.h"

struct VanargandHalfChoices {
    u32 masks[2];
};
extern const struct VanargandHalfChoices gUnk_0824F790;
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u16 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}
void Vanargand_SetPendingCue(void *, u16);
u32 PairedSceneActor_GetActiveMask(struct VanargandAttackPair *);

void sub_081AA330(struct VanargandPairAttack *p)
{
    u32 mask = 4;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending != 0) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                struct VanargandHalfChoices choices;
                switch ((s32)PairedSceneActor_GetActiveMask(p->pair)) {
                    case 0:
                        choices = gUnk_0824F790;
                        PairedSceneActor_ApplyToSelectedHalves(
                            p->pair, choices.masks[(RandomValue() >> 4) & 1], 1);
                        break;
                    case 1:
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 1);
                        break;
                    case 2:
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 1);
                        break;
                }
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
