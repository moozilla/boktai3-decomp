// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 91.7%
// fix.py best version (reprinted C)
struct HomingVec {
    s16 x, y, z;
};
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
    struct HomingVec position;
    u8 unknown24[8];
};
struct HomingSlot {
    u32 lifetime;
    u8 unknown04[8];
    struct HomingVec velocity;
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
extern u32 gUnk_030053F0, gUnk_02000560;
extern const s16 gUnk_086149C4[];
void HomingProjectile_ReleaseSlot(struct HomingManager *, struct HomingSlot *, s32);

static inline u32 TestPause(u32 mask)
{
    return (gEngineFlags | gUnk_030053F0) & mask;
}
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline void Vector(struct HomingVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}

s32 HomingProjectile_UpdateManager(struct HomingManager *m)
{
    struct HomingSlot *p = m->slots;
    s32 i;
    s32 slot;
    u32 k1;
    for (i = 0; i <= 2; i++, p++) {
        void **palette = &p->graphics.palette;
        if (m->activeMask & (1 << i)) {
            struct HomingSprite *sprite = &p->sprite;
            struct HomingParams *params = &p->params;
            register struct HomingVec *velocity asm("ip");
            struct HomingVec *position;
            if (TestPause(0x400))
                continue;
            if (gUnk_02000560 & 4)
                continue;
            slot = i;
            if (params->behavior(p, slot) == 1) {
                HomingProjectile_ReleaseSlot(m, p, slot);
                continue;
            }
            {
                u32 test = p->state & 1;
                u32 *flags = &p->state;
                velocity = &p->velocity;
                asm("" : "+r"(velocity));
                if (test) {
                    Vector(velocity, params->angle, params->speed);
                    *flags &= ~1;
                }
            }
            position = &sprite->position;
            position->x += velocity->x;
            position->y += velocity->y;
            position->z += velocity->z;
            {
                struct DisplaySequenceState *state = &p->sequence;
                const volatile u16 *command = p->sequence.commands + p->sequence.index;
                u32 duration;
                u32 flags;
                sprite->frame = (*command) >> (k1 = 6);
                {
                    u32 input = p->sequence.flags;
                    register u32 left asm("r2") = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != ((((*command) & 0x30) >> 4) & mask))
                        sprite->flags |= 4;
                    else
                        sprite->flags &= ~4;
                }
                {
                    register u32 input asm("r0") = state->flags;
                    u32 mask = 2;
                    register u32 left asm("r1") = (u8)(mask & input);
                    if (left != ((((*command) & 0x30) >> 4) & mask))
                        sprite->flags |= 8;
                    else
                        sprite->flags &= ~8;
                }
                {
                    u32 elapsed = state->elapsed + 1;
                    register u32 mask asm("r5");
                    register u32 temp asm("r2");
                    register u32 savedDuration asm("r2");
                    state->elapsed = elapsed;
                    temp = 0xFFFF;
                    asm("" : "+r"(temp));
                    mask = temp;
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
                            } else {
                                state->index = index - 1;
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
                                    state->index = frameCount - 1;
                                } else {
                                    state->index = 0;
                                }
                            }
                        }
                        command = state->commands + state->index;
                        state->frameWeight = (*command) & 15;
                        {
                            register s32 weight asm("r1") = state->frameWeight;
                            s32 speed = state->speed;
                            speed = (speed * weight) >> k1;
                            state->duration = speed;
                            if (!((u8)speed))
                                state->duration = 1;
                        }
                    }
                }
            }
            {
                struct HomingCollider *collider = &p->secondaryCollider;
                struct HomingVec *where = &p->sprite.position;
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
            if (p->lifetime >= ((slot * 30) + 200))
                HomingProjectile_ReleaseSlot(m, p, slot);
            p->lifetime++;
        }
    }

    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", HomingProjectile_UpdateManager);
#endif
