#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "vanargand_pair_attack.h"
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
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
s32 ScenePair_CountPositiveCounters(void *);
static inline void SetFinalPhase(struct VanargandPairAttack *owner, u32 phase)
{
    owner->phase = phase;
    owner->timer = 0;
}
void Vanargand_UpdateSequencedPairAttack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
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
                struct VanargandAttackPair *pair;
                u32 firstState, secondState;
                pair = Vanargand_GetAttackPair(self);
                firstState = Vanargand_HalfState(pair, 0);
                secondState = Vanargand_HalfState(pair, 1);
                if (ScenePair_CountPositiveCounters(pair) == 2 && firstState == 5 &&
                    secondState == 5)
                    self->flash = 60;
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
                if (Vanargand_HalfState(self->pair, 0) == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 1, 6);
                if (Vanargand_HalfState(self->pair, 1) == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 2, 6);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                SetFinalPhase(p, 4);
            }
            break;
        case 4:
            p->finished = 1;
            break;
    }
}
