#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: command 9 alternates the selected pair halves for six timed attacks.
// MATCHING_CONCESSION: register-pinning
// Scope: byte-for-byte match with one retained register pin.
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
    u8 pad588[0x5B4 - 0x588];
    s16 repeats;
    u8 pad5B6[0x5C2 - 0x5B6];
    s16 mode;
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
void Vanargand_UpdateRepeatedPairAttack(struct Vanargand *p)
{
    u8 result;
    /* Reuse the dispatch scratch storage once the phase value is no longer needed. */
    union {
        s32 phase;
        u32 *timer;
    } state;
    if (p->mode == 1) {
        u32 mask = 0x40000;
        p->flags |= mask;
    }
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 0);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pending) {
        p->pending = result;
        p->phase = result;
        p->timer = result;
        p->repeats = result;
        Vanargand_SetPairedMotionParameter(p->pair, p->parameter);
    }
    state.phase = p->phase;
    switch (state.phase) {
        case 0:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 7);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->selected = (RandomValue() >> 4) & 1;
                p->phase = 1;
                p->timer = state.phase;
            }
            break;
        case 1:
            if (p->timer == 0)
                PairedSceneActor_ApplyToSelectedHalves(p->pair, p->selected + 1, 9);
            if (++p->timer > 50) {
                if (++p->repeats > 5) {
                    {
                        u8 next = 2;
                        p->phase = next;
                        {
                            /* Keep the separate zero load for the sixth-repeat reset. */
                            register u32 zero asm("r0") = 0;
                            p->timer = zero;
                        }
                    }
                } else {
                    p->selected = (p->selected + 1) & 1;
                    p->phase = 1;
                    p->timer = result;
                }
            }
            break;
        case 2:
            if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                if (++p->timer > 15) {
                    p->phase = 3;
                    state.timer = &p->timer;
                    *state.timer = result;
                }
            }
            break;
        case 3:
            p->finished = 1;
            break;
    }
}
