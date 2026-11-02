#include "paired_scene_actor.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: command 9 alternates the selected pair halves for six timed attacks.
struct Vanargand {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 entered;
    u8 pending;
    u8 padB2[0x104 - 0xB2];
    u8 (*callback)(struct Vanargand *);
    u8 pad108[0x56C - 0x108];
    struct VanargandAttackPair *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 pad584;
    u16 parameter;
    u8 pad588[0x5B4 - 0x588];
    s16 repeats;
    u8 pad5B6[0x5C2 - 0x5B6];
    s16 mode;
};
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_SetPairedMotionParameter(u8 *, u32);
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
}
void Vanargand_UpdateRepeatedPairAttack(struct Vanargand *p)
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
