#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_sprite_playback.h"
#include "engine.h"
#include "ram.h"
#include "background_controller.h"
// confidence=low: follows the sound delay and switches to scatter trail at the height limit.
struct Vec {
    s32 x, y, z, w;
};
struct Camera {
    u8 pad[0x44];
    s32 x;
};
struct Source {
    u8 pad[0x28];
    struct Vec position;
    u8 pad38[0x5f4 - 0x38];
    struct Vec offset;
};
struct Emitter {
    void (*update)(struct Emitter *);
    u8 pad4[0x20 - 4];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    u8 pad5C[0x9c - 0x5c];
    struct Vec position, velocity;
    u8 padBC[0xcc - 0xbc];
    s32 yOffset, initialVelocity;
    u8 entered, padD5, phase, padD7;
    u16 timer;
    u8 owner;
};
extern struct Source *gUnk_020004F0[];
s32 Div(s32, s32);
u8 HeightProbe_IsBelowLimit(void *);
void ParticleEmitter_UpdateScatterTrail(struct Emitter *);
static inline void GetPosition(struct Vec *out, s32 index)
{
    if (index > 1) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
    } else {
        struct Source *p = gUnk_020004F0[index];
        if (!p) {
            out->x = 0;
            out->y = 0;
            out->z = 0;
        } else {
            out->x = p->position.x;
            out->y = p->position.y;
            out->z = p->position.z;
        }
    }
}
static inline void GetOffset(struct Vec *out, s32 index)
{
    if (index > 1) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
    } else {
        struct Source *p = gUnk_020004F0[index];
        if (!p) {
            out->x = 0;
            out->y = 0;
            out->z = 0;
        } else {
            out->x = p->offset.x;
            out->y = p->offset.y;
            out->z = p->offset.z;
        }
    }
}
static inline u32 TestMode(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
void ParticleEmitter_UpdateFlight(struct Emitter *p)
{
    struct Vec pos, offset;
    if (p->entered) {
        if (TestMode(0x800)) {
            GetPosition(&pos, 1 - p->owner);
            GetOffset(&offset, 1 - p->owner);
        } else {
            GetPosition(&pos, 0);
            GetOffset(&offset, 0);
        }
        p->position.x = pos.x - offset.x + 0xf0000;
        p->position.y = pos.y - pos.z - 0x3ffc0;
        p->position.z = pos.z;
        p->sprite.flags &= ~1;
        p->velocity.x = Div(pos.x - p->position.x, 30) + p->initialVelocity;
        p->velocity.y = Div(pos.y - p->position.y, 20) + 0x2000;
        p->yOffset = 0;
        p->phase = 0;
        p->timer = 0;
        p->entered = 0;
    }
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    p->timer++;
    {
        struct Vec *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        u32 yOff = p->yOffset;
        dst->x = (pos->x >> 12) - gSceneCamera->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    ActorAttachment_StepSprite(&p->sprite, &p->anim);
    p->sprite.flags |= 12;
    if (HeightProbe_IsBelowLimit(p)) {
        p->update = ParticleEmitter_UpdateScatterTrail;
        p->entered = 1;
    }
}
