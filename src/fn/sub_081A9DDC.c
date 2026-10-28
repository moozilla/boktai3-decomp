#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: paired commands 7 and 8 appear to prepare and alternate attacks.
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
    void *callback;
    u8 pad108[0x56C - 0x108];
    void *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 pad584;
    u16 parameter;
};
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void Vanargand_SetPairedMotionParameter(void *, u16);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *);
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
}
void Vanargand_UpdateAlternatingPairAttack(struct Vanargand *p)
{
    struct Vanargand *self = p;
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 0);
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
        p->selected = (RandomValue() >> 3) % 2;
        Vanargand_SetPairedMotionParameter(p->pair, p->parameter);
    }
    if (Callback_InvokeActor(p, p->callback))
        return;
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 7);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                if (p->selected == 0)
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 8);
                else
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 8);
            }
            if (++p->timer > 60) {
                u8 next = 2;
                u32 zero = 0;
                p->phase = next;
                p->timer = zero;
            }
            break;
        case 2:
            if (p->timer == 0) {
                if (p->selected == 0)
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 8);
                else
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 8);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                u8 next = 3;
                u32 zero = 0;
                p->phase = next;
                p->timer = zero;
            }
            break;
        case 3:
            p->finished = 1;
            break;
    }
}
