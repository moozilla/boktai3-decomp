#include "global.h"
#include "display_animation.h"
#include "engine.h"
#include "ram.h"
#include "contact_position.h"
struct HomingSlot;
typedef s32 (*HomingBehavior)(struct HomingSlot *, s32);
struct HomingCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4c];
};
struct HomingGraphics {
    u8 unknown0[6];
    u16 paletteMode;
    u8 unknown8[4];
    void *palette;
    u8 unknown10[0x10];
};
struct HomingParams {
    u8 unknown0[6];
    s8 angle;
    u8 variant;
    s32 speed;
    HomingBehavior behavior;
    u8 unknown10[0x10];
};
struct HomingSprite {
    u32 flags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct ContactPosition position;
    u8 unknown24[8];
};
struct HomingSlot {
    u32 lifetime;
    u8 unknown04[8];
    struct ContactPosition velocity;
    u8 unknown14[0x24 - 0x14];
    struct HomingCollider secondaryCollider;
    struct HomingCollider primaryCollider;
    struct HomingGraphics graphics;
    struct HomingParams params;
    struct HomingSprite sprite;
    u32 state;
    u8 unknown13C[4];
    struct DisplaySequenceState sequence;
    u8 unknown150[0xc];
    s16 flashTimer;
    u8 unknown15E[2];
};
struct HomingManager {
    u8 task[0x18];
    u32 activeMask;
    struct HomingSlot slots[3];
    u8 unknown43C[4];
    u16 palettes[2][16];
};
extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void HomingProjectile_ReleaseSlot(struct HomingManager *, struct HomingSlot *, s32);

static inline u32 TestPause(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline void Vector(struct ContactPosition *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}

static inline u32 BeginMotion(struct HomingSlot *p, struct ContactPosition **velocity, u32 **flags)
{
    u32 moving = p->state & 1;
    *flags = &p->state;
    *velocity = &p->velocity;
    return moving;
}

static inline void ClearFlags(u32 mask, u32 *flags)
{
    *flags &= mask;
}

s32 HomingProjectile_UpdateManager(struct HomingManager *m)
{
    struct HomingSlot *p = m->slots;
    s32 i;
    u32 flagMask = 4;
    for (i = 0; i <= 2; i++, p++) {
        void **palette = &p->graphics.palette;
        if (m->activeMask & (1 << i)) {
            struct HomingSprite *sprite = &p->sprite;
            struct HomingParams *params = &p->params;
            struct ContactPosition *velocity;
            struct ContactPosition *position;
            if (TestPause(0x400))
                continue;
            if (gUnk_02000560 & flagMask)
                continue;
            if (params->behavior(p, i) == 1) {
                HomingProjectile_ReleaseSlot(m, p, i);
                continue;
            }
            {
                u32 *flags;
                u32 test = BeginMotion(p, &velocity, &flags);
                if (test) {
                    Vector(velocity, params->angle, params->speed);
                    ClearFlags(~1, flags);
                }
            }
            position = &sprite->position;
            sprite->position.x += p->velocity.x;
            position->y += velocity->y;
            position->z += velocity->z;
            {
                struct DisplaySequenceState *state = &p->sequence;
                const volatile u16 *command = p->sequence.commands + p->sequence.index;
                u32 duration;
                u32 flags, bit;
                sprite->frame = (*command) >> 6;
                {
                    union DisplaySequenceControl input;
                    u32 first;
                    input.raw = state->flags;
                    bit = 1;
                    first = (u8)(input.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit)) {
                        struct HomingSprite *dst = sprite;
                        dst->flags |= flagMask;
                    } else {
                        struct HomingSprite *dst = sprite;
                        dst->flags &= ~4;
                    }
                }
                {
                    union DisplaySequenceControl input;
                    u32 bit;
                    u32 first;
                    input.raw = state->flags;
                    bit = 2;
                    first = (u8)(input.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit)) {
                        u32 value;
                        {
                            struct HomingSprite *src = sprite;
                            value = src->flags;
                        }
                        {
                            u32 mask = 8;
                            value |= mask;
                        }
                        {
                            struct HomingSprite *dst = sprite;
                            dst->flags = value;
                        }
                    } else {
                        struct HomingSprite *dst = sprite;
                        dst->flags &= ~8;
                    }
                }
                if (++state->elapsed >= state->duration) {
                    state->elapsed = 0;
                    flags = state->flags;
                    if (flags & 4) {
                        if (state->index == 0) {
                            u32 hold = 8;
                            hold &= flags;
                            if (hold)
                                state->elapsed = state->duration;
                            else
                                state->index = state->frameCount - 1;
                        } else {
                            state->index--;
                        }
                    } else {
                        if (++state->index >= state->frameCount) {
                            u32 hold = 8;
                            hold &= flags;
                            if ((u8)hold) {
                                state->elapsed = state->duration;
                                state->index = state->frameCount - 1;
                            } else {
                                state->index = 0;
                            }
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
                        s32 weight = state->frameWeight;
                        s32 speed = state->speed;
                        speed *= weight;
                        speed >>= 6;
                        state->duration = speed;
                        if (!(u8)speed)
                            state->duration = 1;
                    }
                }
            }
            {
                struct HomingCollider *collider = &p->secondaryCollider;
                struct ContactPosition *where = &p->sprite.position;
                Collider_SetPosition(collider, where, 0);
                if (!(p->primaryCollider.flags & 4))
                    Collider_Refresh(collider);
                collider = &p->primaryCollider;
                Collider_SetPosition(collider, where, 0);
            }
            if (p->flashTimer > 0) {
                *palette = m->palettes[1];
                p->graphics.paletteMode = 0xE012;
                p->flashTimer--;
            } else
                *palette = m->palettes[0];
            if (p->lifetime >= ((i * 30) + 200))
                HomingProjectile_ReleaseSlot(m, p, i);
            p->lifetime++;
        }
    }

    return 0;
}
