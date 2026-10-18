#define gSpriteEffectBlinkPeriods gUnk_086106E0
#include "global.h"

struct SpriteEffectBlink {
    u32 flags;
    u8 unk04[0x8c];
    u8 shadow[0x40];
    s32 timer;
    u16 state;
    u8 active;
    s8 slot;
};
struct SpriteEffectOwner {
    u8 unk00[0x34];
    u32 activeEffects;
};
extern const u8 gUnk_086106E0[];
s32 Mod(s32, s32);
void Sprite_Remove(void *);
void Shadow_Destroy(void *);

void SpriteEffect_UpdateBlinkAndExpire(struct SpriteEffectOwner *owner,
                                       struct SpriteEffectBlink *effect)
{
    struct SpriteEffectBlink *p = effect;
    s32 rate;
    if (effect->timer > 60)
        rate = 2;
    else if (effect->timer > 40)
        rate = 1;
    else
        rate = 0;
    if (Mod(effect->timer, gUnk_086106E0[rate]) == 0) {
        if (p->flags & 1)
            p->flags &= ~1;
        else
            p->flags |= 1;
    }
    if (effect->timer > 90) {
        owner->activeEffects &= ~(1 << effect->slot);
        effect->active = 0;
        Sprite_Remove(p);
        Shadow_Destroy(effect->shadow);
        effect->slot = -1;
        effect->state = 0;
        effect->timer = 0;
    } else {
        effect->timer++;
    }
}
