#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: paired commands 3/4/5 repeat a synchronized attack before selected halves recover.
#include "vanargand_pair_attack.h"
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void Vanargand_SetPairedMotionParameter(void *, u16);
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
}
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_UpdateSynchronizedPairAttack(struct VanargandPairAttack *p)
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
        p->repeats = result;
        Vanargand_SetPairedMotionParameter(p->pair, p->parameter588);
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
                if (ScenePair_CountPositiveCounters(self->pair) == 2)
                    Vanargand_SetPairedMotionParameter(self->pair, self->parameter588);
                else
                    Vanargand_SetPairedMotionParameter(self->pair, self->parameter);
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 5);
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
                if (++self->repeats <= 2) {
                    p->phase = 3;
                    p->timer = 0;
                } else {
                    p->phase = 4;
                    p->timer = 0;
                }
            }
            break;
        case 3:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 3);
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
        case 4:
            if (p->timer == 0) {
                if (self->pair->halves[0].requestedState == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 1, 6);
                if (self->pair->halves[1].requestedState == 5)
                    PairedSceneActor_ApplyToSelectedHalves(self->pair, 2, 6);
                Vanargand_SetPairedMotionParameter(self->pair, self->parameter584);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                {
                    u8 next = 5;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 5:
            p->finished = 1;
            break;
    }
}
