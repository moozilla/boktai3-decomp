#define gSceneEffectBlinkPeriods gUnk_086106C4
#include "global.h"

struct SceneEffectBlink {
    u32 flags;
    u8 unk04[0x96];
    u8 active;
    s8 slot;
    u8 unk9C[4];
    s16 timer;
};
struct SceneEffectOwner {
    u8 unk00[0x38];
    u32 activeEffects;
};
extern const u8 gUnk_086106C4[];
s32 Mod(s32, s32);

void SceneEffect_UpdateBlink(struct SceneEffectOwner *owner, struct SceneEffectBlink *effect)
{
    struct SceneEffectBlink *p = effect;
    s32 rate;
    if (effect->timer > 60)
        rate = 2;
    else if (effect->timer > 40)
        rate = 1;
    else
        rate = 0;
    if (Mod(effect->timer, gUnk_086106C4[rate]) == 0) {
        if (p->flags & 1)
            p->flags &= ~1;
        else
            p->flags |= 1;
    }
    if (effect->timer > 90) {
        p->flags |= 1;
        owner->activeEffects &= ~(1 << effect->slot);
        effect->active = 0;
    }
    effect->timer++;
}
