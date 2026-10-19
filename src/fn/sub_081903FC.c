#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.7%
// foundry draft (worker best 93.3%); what differs is mostly register allocation and instruction order
#define Nidhoggr_UpdateDistanceLimitedEffect sub_081903FC
#define gNidhoggrEffectPhaseDurations gUnk_0824F292
#define gNidhoggrEffectPhaseFrames gUnk_0824F28C
struct NidhoggrDistanceEffect {
    void *update;
    void *destroy;
    u8 pad8[0x2c];
    u16 frame;
    u8 pad36[0xa];
    s16 x;
    u8 pad42[0x136];
    s16 speed;
    u8 pad17A[6];
    u8 phase, phaseTimer;
    s16 startX;
};
extern const u8 gUnk_0824F292[], gUnk_0824F28C[];
s32 Callback_InvokeActor(void *, void *);
void sub_081903FC(struct NidhoggrDistanceEffect *effect)
{
    if (effect->x - effect->startX <= 0x1000) {
    if (effect->phase > 5 && effect->speed > 8) effect->speed--;
    if (effect->phaseTimer < gNidhoggrEffectPhaseDurations[effect->phase]) return;
        if (effect->phase <= 5) goto advance;
    }
    {
            effect->update = 0;
            if (effect->destroy) {
                Callback_InvokeActor(effect, effect->destroy);
                effect->destroy = 0;
            }
        return;
    }
    advance:
    {
            effect->phase++;
            if (effect->phase <= 5) effect->frame = gNidhoggrEffectPhaseFrames[effect->phase];
            effect->phaseTimer = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081903FC);
#endif
