#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: paired commands and positive-counter checks appear to gate a charged attack.
// MATCHING_CONCESSION: register-pinning
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
void Vanargand_UpdateChargedPairAttack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    u8 result;
    if (p->mode == 1) {
        u32 mask = 0x40000;
        p->flags |= mask;
    }
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pending) {
        p->pending = result;
        p->phase = result;
        p->timer = result;
        p->repeats = result;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, self->animation, self->drawable, 4, 1, 0);
                Sound_PlaySong(0x586);
                p->timer++;
            }
            self->delay = 2;
            if (p->padA3 > 3) {
                p->phase = 1;
                {
                    u32 zero = 0;
                    p->timer = zero;
                }
            }
            break;
        case 1:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x13);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                u8 next = 2;
                u32 zero = 0;
                p->phase = next;
                p->timer = zero;
            }
            break;
        case 2:
            if (p->timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0xC);
                p->timer++;
            }
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                struct VanargandAttackPair *pair;
                register u32 a asm("r4");
                u32 b;
                asm("" : "+r"(self));
                pair = self->pair;
                a = pair->first;
                b = pair->second;
                if (ScenePair_CountPositiveCounters(pair) == 2 && a == 0xC && b == 0xC)
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
            if (p->timer == 0)
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x17);
            if (++p->timer > 119) {
                u8 next = 4;
                u32 zero = 0;
                p->phase = next;
                p->timer = zero;
            }
            break;
        case 4:
            p->finished = 1;
            break;
    }
}
