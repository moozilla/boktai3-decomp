#include "global.h"
#include "actor_attachment_state.h"
#include "ram.h"
#include "background_controller.h"
#include "player_cue_sequence.h"
static inline u32 ActorAttachment_StepSprite(struct ContactSwarmSprite *sprite,
                                             struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}

struct Vec {
    s32 x, y, z, w;
};
struct Packed {
    s16 x, y, z, pad;
};
struct Camera {
    u8 pad[0x44];
    s32 x;
};
struct Effect {
    void (*update)(struct Effect *);
    u8 collider[0x5c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    void *data;
    struct Vec position, velocity;
    s32 height, speed;
    u16 lifetime;
    u8 angle, state;
    u16 id;
    u8 entered, active;
    s32 direction;
    u32 timer;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_086141BC[];
s32 Mod(s32, s32);
void NativeParticle_CreateWithFlag(void *, s32, void *, s32, s32, s32, s32);
void NativeEffect_SetState(struct Effect *, void (*)(struct Effect *));
void SpriteEffect_RemoveWithBurst(struct Effect *);
void NativeEffect_DestroyEntry(struct Effect *);
u8 SceneGroundEffect_IsBelowGround(struct Effect *, s32);
void Collider_SetFixedPositionWithYOffset(void *, struct Vec *, s32);
void SpatialIndex_InsertEntry(void *);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Direction(struct Packed *out, u8 angle, s32 speed)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle] * speed);
}
static inline void Project(struct Packed *out, struct Vec *pos, s32 h)
{
    out->x = (pos->x >> 12) - gSceneCamera->x;
    out->y = ((s32)(pos->z + (pos->y + h)) >> 12) + 0x60;
    out->z = pos->z >> 12;
}
// confidence=low: type zero and threshold sprites travel at fixed velocity.
void NativeEffect_UpdateLinearFlight(struct Effect *p)
{
    struct Packed projected;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->entered = 0;
    }
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    Project(&projected, &p->position, p->height);
    *(struct Packed *)&p->sprite.position = projected;
    ActorAttachment_StepSprite(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->height);
    SpatialIndex_InsertEntry(p->collider);
    if (SceneGroundEffect_IsBelowGround(p, 8)) {
        NativeEffect_SetState(p, NativeEffect_DestroyEntry);
    } else if (!--p->lifetime) {
        NativeEffect_SetState(p, NativeEffect_DestroyEntry);
    }
}
