#include "bosses/vanargand/vanargand_pair_attack.h"
#include "bosses/vanargand/paired_scene_actor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
// confidence=low: command 9 alternates the selected pair halves for six timed attacks.
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_SetPairedMotionParameter(u8 *, u32);
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
}
void Vanargand_UpdateRepeatedPairAttack(struct VanargandPairAttack *p)
{
    s32 callbackResult;
    s32 phase;
    if (p->mode == 1) {
        u32 mask = 0x40000;
        p->flags |= mask;
    }
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 0);
    callbackResult = p->callback(p);
    if ((s16)callbackResult > 0)
        return;
    if (p->pending) {
        p->pending = callbackResult;
        p->phase = callbackResult;
        p->timer = callbackResult;
        p->repeats = callbackResult;
        Vanargand_SetPairedMotionParameter((u8 *)p->pair, p->parameter);
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 7);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->selected = (RandomValue() >> 4) & 1;
                p->phase = 1;
                p->timer = phase;
            }
            break;
        case 1:
            if (p->timer == 0)
                PairedSceneActor_ApplyToSelectedHalves(p->pair, p->selected + 1, 9);
            if (++p->timer > 50) {
                if (++p->repeats > 5) {
                    p->phase = 2;
                    p->timer = 0;
                } else {
                    p->selected = (p->selected + 1) & 1;
                    p->phase = 1;
                    p->timer = callbackResult;
                }
            }
            break;
        case 2:
            if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                if (++p->timer > 15) {
                    p->phase = 3;
                    p->timer = callbackResult;
                }
            }
            break;
        case 3:
            p->finished = 1;
            break;
    }
}
