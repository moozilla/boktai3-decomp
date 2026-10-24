// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: state dispatch, coordinate lifetimes and register allocation.
#include "global.h"
#include "display_animation.h"
struct NidhoggrVector {
    s16 x, y, z, pad;
};
struct NidhoggrSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct NidhoggrVector position;
    u8 pad24[8];
};
struct NidhoggrEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct NidhoggrSprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x114];
    void *anchor;
    struct NidhoggrVector velocity;
    u8 state, age;
    s16 ground;
    u8 *boss;
};
#ifndef NIDHOGGR_MASK
#define NIDHOGGR_MASK "r5"
#endif
static inline u32 Nidhoggr_StepAnimation(struct NidhoggrEffect *owner)
{
    struct NidhoggrSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state = &owner->sequence;
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
        register u32 first asm("r1");
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
        register u32 mask asm(NIDHOGGR_MASK);
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Preserve the observed constant-to-mask copy. */

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

void Nidhoggr_CreatePalette426Effect(void *, void *);
void Nidhoggr_CreatePalette424Effect(void *, void *);
void Nidhoggr_CreateTimedPositionEffect(void *, void *);
void EffectPool_CreateTimedEffect(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ScaledShadow_Enable(void *);
void Sound_PlaySongChecked(u32);

struct TerrainRows {
    u8 pad[4];
    u8 *cells;
    u8 pad8[0x1c];
    u16 rows[1];
};

extern u32 gUnk_030052F8, gUnk_030052FC;

u8 *TerrainModifiers_FindCellOverride(int, int);
static inline u8 *cell(int index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + index * 4;
}
// confidence=low: rising effect retargets the player before descending and emitting an impact.
inline static u32 Nidhoggr_StepAnimation_UpdateFallingAttackEffect(struct NidhoggrEffect *owner)
{
    struct NidhoggrSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state = &owner->sequence;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    u32 k1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & (k1 = 0x30)) >> 4) & bit))
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
        if (first != ((((*command) & k1) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

void Nidhoggr_UpdateFallingAttackEffect(struct NidhoggrEffect *p)
{
    u8 *age;
    u8 *state;
    Nidhoggr_StepAnimation_UpdateFallingAttackEffect(p);
    age = &p->age;
    if (!((*age) & 3))
        Nidhoggr_CreatePalette426Effect(p->boss + 0x54, &p->sprite.position);
    state = &p->state;
    switch (*state) {
        case 0:
            if ((*age) > 15) {
                *state = 1;
                p->anchor = 0;
                p->velocity.x = 0;
                p->velocity.y = 0x33;
                p->velocity.z = 0;
                p->ground = p->sprite.position.y;
            } else
                p->velocity.y += 0x33;
            break;

        case 1:
            if ((p->sprite.position.y - p->ground) > 0x1000) {
                u8 *boss = p->boss;
                struct NidhoggrVector *position;
                struct NidhoggrVector *savedPosition;
                u16 *xField;
                u16 *zField;
                u16 value;
                int x;
                int z;
                int index;
                int slope;
                int height;
                u8 *c;
                struct NidhoggrVector v;
                value = *((u16 *)(gUnk_02000710 + 0x30));
                xField = (u16 *)(&p->sprite.position.x);
                value = *((u16 *)(gUnk_02000710 + 0x34));
                *xField = value;
                zField = (u16 *)(&p->sprite.position.z);
                *((u16 *)(&p->sprite.position.z)) = value;
                p->velocity.y = -0x33;
                (*state)++;
                Anim_RestartSequence(&p->sequence, *((void **)(boss + 0x5400)), 4, 0, 0);
                position = &p->sprite.position;
                x = position->x >> 8;
                z = position->z >> 8;
                savedPosition = position;
                asm("" : "+r"(savedPosition));
                if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) ||
                    (((u32)z) >= gUnk_030052FC))
                    index = 0;
                else
                    index = gUnk_030052F4->rows[z] + x;
                c = cell(index);
                slope = (*c) >> 4;
                height = ((*c) & 15) << 8;
                switch (slope) {
                    case 1:
                        height -= (u8)position->z;
                        break;

                    case 2:
                        height -= (u8)position->x;
                        break;
                }

                p->ground = height;
                {
                    struct EffectCollider {
                        u8 pad[6];
                        u16 flags;
                    };
                    struct EffectCollider *collider = (void *)(((u8 *)p) + 0xcc);
                    u32 mask = ~4;
                    collider->flags &= mask;
                }
                ScaledShadow_Enable(((u8 *)p) + 0x60);
                v.x = savedPosition->x;
                v.y = p->ground;
                v.z = *((u16 *)(&p->sprite.position.z));
                EffectPool_CreateTimedEffect(boss + 0x54, &v);
            }
            break;

        case 2:
            if ((p->sprite.position.y - p->ground) <= 0x87f) {
                Sound_PlaySongChecked(0x1b5);
                (*state)++;
            }

        case 3:
            if (p->sprite.position.y < (p->ground + 0x100)) {
                u8 *boss = p->boss;
                struct NidhoggrVector v;
                v.x = p->sprite.position.x;
                v.y = p->sprite.position.y;
                v.z = p->sprite.position.z;
                if (p->ground <= 0) {
                    v.y -= 0x100;
                    Nidhoggr_CreatePalette424Effect(boss + 0x54, &v);
                } else {
                    v.y -= 0x80;
                    Nidhoggr_CreateTimedPositionEffect(&v, boss);
                }
                goto destroy;
            }
            break;

        case 255: {
            u8 *boss = p->boss;
            Nidhoggr_CreateTimedPositionEffect(&p->sprite.position, boss);
        }
        destroy:
            p->update = 0;

            if (p->destroy) {
                Callback_InvokeActor(p, p->destroy);
                p->destroy = 0;
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Nidhoggr_UpdateFallingAttackEffect);
#endif
