// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
struct Pair {
    u8 pad0[0x205];
    u8 first;
    u8 pad206[0x421 - 0x206];
    u8 second;
};
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
    u8 pad108[0x18C - 0x108];
    u8 animation[0x60];
    u8 drawable[0x56C - 0x1EC];
    struct Pair *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 parameter584;
    u16 parameter;
    u16 parameter588;
    u8 pad58A[0x5B4 - 0x58A];
    s16 repeats;
    u8 pad5B6[0x5C2 - 0x5B6];
    s16 mode;
};
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void Vanargand_SetPairedMotionParameter(void *, u16);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *);
extern u16 gUnk_0203B400[];
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
}
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
s32 ScenePair_CountPositiveCounters(void *);
void Vanargand_UpdateSequencedPairAttack(struct Vanargand *p)
{
    struct Vanargand *self = p;
    u8 result;
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 0);
    if (p->mode == 1) {
        u32 mask = 0x40000;
        p->flags |= mask;
    }
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pending) {
        p->pending = result;
        p->phase = result;
        p->timer = result;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 3);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                {
                    u8 next = 1;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 1:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 4);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                {
                    u8 next = 2;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 2:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 5);
                if (ScenePair_CountPositiveCounters(self->pair) == 2)
                    Vanargand_SetPairedMotionParameter(self->pair, self->parameter588);
                else
                    Vanargand_SetPairedMotionParameter(self->pair, self->parameter);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                struct Pair *pair;
                register u32 a asm("r4");
                u32 b;
                asm("" : "+r"(self));
                pair = self->pair;
                a = pair->first;
                b = pair->second;
                if (ScenePair_CountPositiveCounters(pair) == 2 && a == 5 && b == 5)
                    *(u16 *)((u8 *)self + 0xF3C) = 60;
                {
                    u8 next = 3;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 3:
            if (p->timer == 0) {
                if (self->pair->first == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 1, 6);
                if (self->pair->second == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 2, 6);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(({
                           asm("" : : "r"(p));
                           self->pair;
                       }))) {
                {
                    u8 next = 4;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 4:
            p->finished = 1;
            break;
    }
}
