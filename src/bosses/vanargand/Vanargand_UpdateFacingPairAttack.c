#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/engine_globals.h"
// confidence=low: facing and active-half mask choose the side for randomized commands 8/11.
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void Vanargand_SetPairedMotionParameter(struct VanargandAttackPair *, u16);
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
u32 PairedSceneActor_GetActiveMask(struct VanargandAttackPair *);
static inline u8 preferSecond(struct VanargandPairAttack *p)
{
    u32 direction = p->targetBearings[0] - 32;
    u8 result = 1;
    if (direction <= 127)
        result = 0;
    return result;
}
void Vanargand_UpdateFacingPairAttack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
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
                struct VanargandAttackPair **pp;
                s32 bits = (RandomValue() >> 4) & 3;
                command = 11;
                if (bits)
                    command = 8;
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
                    p->phase = next;
                    p->timer = 0;
                }
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
}
