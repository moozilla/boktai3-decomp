#include "particle_effect_state.h"

/* tentative: per-frame active state: counts down, re-arms the collider and restarts the animation */

#include "player_cue_sequence.h"
#include "background_controller.h"

struct BurstCamera {
    u8 pad[0x44];
    s32 x;
};
extern const u8 gUnk_086108E8[];
void Memory_Clear(void *, s32);
void Collider_SetFixedPositionWithYOffset(void *, struct BurstPos *, u32);
void SpatialIndex_InsertEntry(void *);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void ParticleBurstEffect_SetStateCallback(struct BurstEntry *, void (*)(struct BurstEntry *));
void SpriteEffect_HideWithScatterBurst(struct BurstEntry *);

void ParticleBurstEffect_TickActive(struct BurstEntry *p)
{
    s32 level;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->entered = 0;
        Memory_Clear(p->velocity, sizeof(p->velocity));
        Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->yOffset);
    }
    if (p->timer) {
        p->timer--;
    } else {
        if (p->duration < 30)
            level = 3;
        else if (p->duration < 50)
            level = 2;
        else if (p->duration < 90)
            level = 1;
        else
            goto skip;
        Anim_SetSequence(&p->anim, p->data, 7, (u8)p->value, 0);
        p->value ^= 1;
        p->timer = gUnk_086108E8[level];
    }
skip: {
    struct BurstPos *pos = &p->position;
    struct ContactPosition *dst = &p->sprite.position;
    u32 yOff = (u32)p->yOffset;
    dst->x = (pos->x >> 12) - ((struct BurstCamera *)gSceneCamera)->x;
    dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
    dst->z = pos->z >> 12;
}
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    SpatialIndex_InsertEntry(p->collider);
    if (--p->duration == 0)
        ParticleBurstEffect_SetStateCallback(p, SpriteEffect_HideWithScatterBurst);
}
