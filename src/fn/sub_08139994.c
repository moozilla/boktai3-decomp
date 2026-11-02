#include "particle_effect_state.h"

/* tentative: falls under gravity, lands with a sound and switches to the active state */

#include "player_cue_sequence.h"
#include "background_controller.h"

struct BurstCamera {
    u8 pad[0x44];
    s32 x;
};
void Memory_Clear(void *, s32);
void Sound_PlaySong(u32);
void Collider_SetFixedPositionWithYOffset(void *, struct BurstPos *, s32);
void SpatialIndex_InsertEntry(void *);
void ParticleBurstEffect_SetStateCallback(struct BurstEntry *, void (*)(struct BurstEntry *));
void ParticleBurstEffect_TickActive(struct BurstEntry *);

void ParticleBurstEffect_UpdateFalling(struct BurstEntry *p)
{
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->entered = 0;
        Memory_Clear(p->velocity, sizeof(p->velocity));
    }
    p->velocity[1] += 0x200;
    p->yOffset += p->velocity[1];
    if (p->yOffset >= 0) {
        if (!p->state) {
            Sound_PlaySong(0x3e6);
            p->state = 1;
        }
        p->yOffset = 0;
        ParticleBurstEffect_SetStateCallback(p, ParticleBurstEffect_TickActive);
    }
    {
        struct BurstPos *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        s32 yOff = p->yOffset;
        dst->x = (pos->x >> 12) - ((struct BurstCamera *)gSceneCamera)->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->yOffset);
    SpatialIndex_InsertEntry(p->collider);
}
