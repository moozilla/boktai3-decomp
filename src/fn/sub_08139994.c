// MATCHING_CONCESSION: register-pinning
#include "global.h"

/* tentative: falls under gravity, lands with a sound and switches to the active state */

#include "actor_attachment_sprite_playback.h"
#include "background_controller.h"

struct BurstPos {
    s32 x, y, z, w;
};
struct BurstCamera {
    u8 pad[0x44];
    s32 x;
};
struct BurstEntry {
    u8 pad0[4];
    u8 collider[0x5c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    void *data;
    struct BurstPos position;
    s32 velocity[4];
    s32 yOffset;
    u8 padC4[4];
    u16 duration;
    u8 active;
    u8 dirty;
    u16 id;
    u8 timer;
    u8 state;
    u32 value;
};
void Memory_Clear(void *, s32);
void Sound_PlaySong(u32);
void Collider_SetFixedPositionWithYOffset(void *, struct BurstPos *, s32);
void SpatialIndex_InsertEntry(void *);
void ParticleBurstEffect_SetStateCallback(struct BurstEntry *, void (*)(struct BurstEntry *));
void ParticleBurstEffect_TickActive(struct BurstEntry *);

void ParticleBurstEffect_UpdateFalling(struct BurstEntry *p)
{
    if (p->dirty) {
        p->sprite.flags &= ~1;
        p->dirty = 0;
        Memory_Clear(p->velocity, 0x10);
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
    ActorAttachment_StepSprite(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->yOffset);
    SpatialIndex_InsertEntry(p->collider);
}
