#include "global.h"
#include "engine_globals.h"
// confidence=low: facing and active-half mask choose the side for randomized commands 8/11.
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
    s16 selectedHalf;
    u8 pad5B8[0x5C2 - 0x5B8];
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
    {
        u32 offset = gRandomIndex * 2;
        return *(u16 *)(offset + RANDOM_TABLE_ADDR);
    }
}
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
s32 ScenePair_CountPositiveCounters(void *);
u32 PairedSceneActor_GetActiveMask(void *);
static inline u8 preferSecond(struct Vanargand *p)
{
    u32 direction = *(u8 *)((u8 *)p + 0xCC) - 32;
    u8 result = 1;
    if (direction <= 127)
        result = 0;
    return result;
}
void Vanargand_UpdateFacingPairAttack(struct Vanargand *p)
{
    struct Vanargand *self = p;
    s32 phase;
    u8 result;
    if (p->mode == 1) {
        u32 mask = 0x40000;
        p->flags |= mask;
    }
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    Vanargand_SetPendingCue(p, 0);
    if (p->pending) {
        p->pending = result;
        p->phase = result;
        p->timer = result;
        Vanargand_SetPairedMotionParameter(p->pair, p->parameter);
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 7);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->phase = 1;
                p->timer = phase;
            }
            break;
        case 1:
            Vanargand_SetPendingCue(p, 2);
            if (p->timer == 0) {
                s32 command;
                u32 active;
                struct Pair **pp;
                {
                    s32 bits = (RandomValue() >> 4) & 3;
                    command = 11;
                    if (bits)
                        command = 8;
                }
                pp = &p->pair;
                active = PairedSceneActor_GetActiveMask(*pp);
                if (!preferSecond(p)) {
                    if (active & 1) {
                        PairedSceneActor_ApplyToSelectedHalves(*pp, 1, command);
                        p->selectedHalf = 1;
                    } else if (active & 2) {
                        PairedSceneActor_ApplyToSelectedHalves(*pp, 2, command);
                        p->selectedHalf = 2;
                    } else {
                        p->phase = 2;
                        p->timer = 0;
                        break;
                    }
                } else {
                    if (active & 2) {
                        PairedSceneActor_ApplyToSelectedHalves(*pp, 2, command);
                        p->selectedHalf = 2;
                    } else if (active & 1) {
                        PairedSceneActor_ApplyToSelectedHalves(*pp, 1, command);
                        p->selectedHalf = 1;
                    } else {
                        p->phase = 2;
                        p->timer = 0;
                        break;
                    }
                }
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                Vanargand_SetPairedMotionParameter(self->pair, self->parameter584);
                {
                    u8 next = 2;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
}
