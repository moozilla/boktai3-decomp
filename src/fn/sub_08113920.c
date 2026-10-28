// MATCHING_CONCESSION: register-pinning
#include "global.h"
// fix.py best version (reprinted C)
// confidence=low: profile-driven native effect motion and creation.
/* Pointer spills, ring-mask allocation and a hoisted constant still differ. */
#include "actor_attachment_state.h"
#include "ram.h"
#include "background_controller.h"
static inline u32 ActorAttachment_StepSprite(struct ContactSwarmSprite *sprite,
                                             struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
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
static inline u32 TrailRandom(void)
{
    register u32 value asm("r0");
    typeof(value) t1;
    value = (t1 = gUnk_086141BC[gEffectRandomTableIndex]);
    asm("" : "+r"(t1));
    value &= 7;
    asm("" : "+r"(value));
    return value;
}

static inline u8 Odd(u32 t)
{
    return t & 1;
}
// confidence=low: horizontal motion accelerates and leaves a short particle trail.
static inline u32
ActorAttachment_StepSprite_UpdateAcceleratingTrail(struct ContactSwarmSprite *sprite,
                                                   struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

inline static u8 Odd_UpdateAcceleratingTrail(u32 t)
{
    {
        u32 k1 = 1;
        return t & k1;
    }
}

void NativeEffect_UpdateAcceleratingTrail(struct Effect *p)
{
    struct Packed projected;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->timer = 0;
        p->entered = 0;
    }
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    Project(&projected, &p->position, p->height);
    *((struct Packed *)(&p->sprite.position)) = projected;
    ActorAttachment_StepSprite_UpdateAcceleratingTrail(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->height);
    SpatialIndex_InsertEntry(p->collider);
    p->velocity.x += p->direction << 9;
    if ((p->timer <= 9) && Odd_UpdateAcceleratingTrail(p->timer)) {
        struct {
            u32 x : 16;
            u32 y : 16;
            u32 z : 16;
            u32 pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = TrailRandom() - 3;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = TrailRandom() - 3;
        NativeParticle_CreateWithFlag(&p->sprite.position, 0, &offset, 1, 1, 0x200, 0);
    }
    p->timer++;
    if (SceneGroundEffect_IsBelowGround(p, 0)) {
        NativeEffect_SetState(p, SpriteEffect_RemoveWithBurst);
    } else if (!(--p->lifetime)) {
        NativeEffect_SetState(p, SpriteEffect_RemoveWithBurst);
    }
}
