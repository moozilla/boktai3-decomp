#include "global.h"
#include "actor_attachment_state.h"
#include "ram.h"
#include "background_controller.h"
#include "player_cue_sequence.h"
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
static inline void Direction(struct Packed *out, u16 angle, s32 speed)
{
    s16 a = angle;
    out->x = Scale(gUnk_086149C4[(a + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle] * speed);
}
static inline void Project(struct Packed *out, struct Vec *pos, s32 h)
{
    out->x = (pos->x >> 12) - gSceneCamera->x;
    out->y = ((s32)(pos->z + (pos->y + h)) >> 12) + 0x60;
    out->z = pos->z >> 12;
}
static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}
// confidence=low: type three moves along a changing heading and emits trail particles.
void NativeEffect_UpdateTurningTrail(struct Effect *p)
{
    struct Packed velocity, projected;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->timer = 0;
        p->entered = 0;
    }
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    if (p->timer <= 5) {
        Direction(&velocity, p->angle, 2);
        p->velocity.x = (velocity.x << 12) + p->speed;
        p->velocity.z = velocity.z << 12;
    } else if (p->timer <= 19) {
        p->angle += p->direction * 2;
        if (p->timer == 19)
            p->angle = 0;
        Direction(&velocity, p->angle, 1);
        p->velocity.x = (velocity.x << 12) + p->speed;
        p->velocity.z = velocity.z << 12;
    } else {
        Direction(&velocity, p->angle, 3);
        if (!Mod(p->timer, 4))
            p->speed += 0x1000;
        p->velocity.x = (velocity.x << 12) + p->speed;
        p->velocity.z = velocity.z << 12;
    }
    Project(&projected, &p->position, p->height);
    *(struct Packed *)&p->sprite.position = projected;
    if (p->timer <= 19 && (p->timer & 1)) {
        struct {
            u32 x : 16, y : 16, z : 16, pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = (RandomValue() & 15) - 7;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = (RandomValue() & 15) - 7;
        NativeParticle_CreateWithFlag(&p->sprite.position, 0, &offset, 1, 1, 0x200, 0);
    }
    p->timer++;
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->height);
    SpatialIndex_InsertEntry(p->collider);
    if (SceneGroundEffect_IsBelowGround(p, 0)) {
        NativeEffect_SetState(p, SpriteEffect_RemoveWithBurst);
    } else if (!--p->lifetime) {
        NativeEffect_SetState(p, SpriteEffect_RemoveWithBurst);
    }
}
