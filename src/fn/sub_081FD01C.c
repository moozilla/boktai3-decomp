// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"

#define PROJECTILE_RANDOM_TABLE ((const u16 *)0x0203B400)

struct Pos {
    s16 x, y, z, unk_06;
};
struct Pair {
    s32 a, b;
};
struct Obj {
    u32 flags;
    u8 unk_04[3], priority;
    u8 unk_08[8]; /* The first two bytes hold the sprite scales. */
    u16 frame;
    u8 unk_12[10];
    struct Pos pos;
    u8 unk_24[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Params {
    u16 unk_00, lifetime, unk_04;
    s8 angle;
    u8 priority;
    s32 speed;
    u32 unk_0C;
    void *callback;
    u32 attackType, attackFlags;
    s32 attack, element, collisionFlags;
    struct Pair extent, offset;
    u32 unk_38, resource, frame;
    s32 bank;
    u32 script;
    u16 sequence;
    s16 variant;
    u16 unk_50, unk_52, unk_54, unk_56;
};
struct Collider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[2];
    u16 unk_0A;
    u8 unk_0C[8];
    struct Pair offset, extent;
    u8 unk_24[0x14];
    u32 flags_38;
    u8 unk_3C[0x18];
};
struct Entry {
    u32 timer;
    u8 unk_04[0xC];
    s16 vx, vy, vz, unk_16;
    struct Anim anim;
    struct Params params;
    u8 animData[0x1C];
    struct Obj sprite;
    struct Collider collider, secondaryCollider;
    s32 speed;
    u8 priority, unk_175;
    u16 lifetime, unk_178, scale;
    void *scriptProc;
    u8 unk_180[0x40];
};
struct Pool {
    u8 unk_00[0x18];
    u32 mask;
};
struct Behaviors {
    void *callback[3];
};
extern const struct Behaviors gUnk_0824FE28;
extern const s16 gUnk_086149C4[];
extern u32 gRandomIndex;
void SceneProjectile_CreatePool(void);
struct Entry *Projectile_FindFreeSlot(struct Pool *, s32 *);
void Projectile_SetBehaviorCallback(struct Params *, void *);
void Memory_Copy(void *, const void *, s32);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_LinkToList(void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

/* Scale a signed 12-bit fixed-point product toward zero. */
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

/* Apply packed frame/flip commands, then advance the sequence timer once. */
static inline void StepPlayback(struct Entry *e, struct Obj *sprite, struct Anim *state)
{
    struct Obj *copy = sprite;
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->frame << 1;
        const volatile u16 *table;
        table = e->anim.data;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            register u32 flags asm("r0") = copy->flags | 8;
            register struct Obj *target asm("r2") = copy;
            asm("" : "+r"(target));
            target->flags = flags;
        } else {
            register struct Obj *target asm("r3") = copy;
            target->flags &= ~8;
        }
    }
    {
        u32 elapsed = state->timer + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->timer = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->rate;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->timer = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->frame;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->timer = savedDuration;
                    } else {
                        state->frame = state->count - 1;
                    }
                } else {
                    state->frame = index - 1;
                }
            } else {
                u32 index = state->frame + 1;
                u32 frameCount;
                u32 endFlag;
                state->frame = index;
                frameCount = state->count;
                if ((index & mask) >= frameCount) {
                    endFlag = 8;
                    endFlag &= flags;
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->timer = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->frame = lastIndex;
                        }
                    } else {
                        state->frame = 0;
                    }
                }
            }
            {
                u32 offset = state->frame << 1;
                const volatile u16 *table;
                table = state->data;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->factor = *command & 15;
            {
                register s32 weight asm("r1") = state->factor;
                s32 speed;
                speed = state->scale;
                speed = (speed * weight) >> 6;
                state->rate = speed;
                if (!(u8)speed)
                    state->rate = 1;
            }
        }
    }
}

/* Variant 0 uses direction zero, variant 1 uses eight directions, and
 * variant 2 uses a random four-way sequence and the secondary collider. */
s32 Projectile_SpawnAtPosition(struct Params *params, struct Pos *pos)
{
    struct Behaviors behaviors = gUnk_0824FE28;
    struct Pool *pool = gSceneProjectilePool;
    struct Entry *e;
    s32 slot;
    u8 *q;
    s32 magnitude;
    s32 angle;
    u32 zero;
    struct Obj *o;
    if (!pool) {
        SceneProjectile_CreatePool();
        return -1;
    }
    e = Projectile_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Projectile_SetBehaviorCallback(params, behaviors.callback[params->variant]);
    Memory_Copy(&e->params, params, 0x58);
    e->timer = 0;
    q = (u8 *)&e->vx;
    angle = params->angle;
    magnitude = params->speed;
    ((struct Pos *)q)->x = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    ((struct Pos *)q)->y = 0;
    ((struct Pos *)q)->z = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
    zero = 0;
    q = (u8 *)&e->collider;
    Collider_SetAttackProperties(q, params->attack, params->element, params->attackType,
                                 params->attackFlags, params->collisionFlags);
    ((struct Collider *)q)->extent = params->extent;
    ((struct Collider *)q)->offset = params->offset;
    Collider_SetPosition(q, pos, 0);
    if (e->params.variant == 2) {
        struct Collider *collider = &e->secondaryCollider;
        Collider_SetPosition(collider, pos, 0);
        Collider_Refresh(collider);
        collider->flags &= 0xFFFB;
    }
    e->sprite.pos = *pos;
    q = e->animData;
    AnimData_Load(q, params->resource);
    {
        struct Obj *t = &e->sprite;
        s32 bank;
        t->frame = params->frame;
        bank = params->bank;
        o = t;
        if (bank >= 0)
            AnimData_SelectPaletteBank(q, bank);
    }
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        void *proc = ScriptProc_Start(0x922E, (u16)e->params.script);
        e->scriptProc = proc;
        if (e->params.variant == 0) {
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, proc, (u16)e->params.sequence, 0, zero);
        } else if (e->params.variant == 1) {
            u32 var = (((u16)e->params.angle + 0x70) >> 5) & 7;
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, proc, (u16)e->params.sequence, var, zero);
        } else {
            u32 var;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            var = PROJECTILE_RANDOM_TABLE[gRandomIndex];
            var &= 3;
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, proc, (u16)e->params.sequence, var, zero);
        }
        StepPlayback(e, o, (struct Anim *)q);
    }
    e->speed = params->speed;
    e->priority = params->priority;
    e->lifetime = params->lifetime;
    e->collider.unk_0A = 0x10;
    e->collider.flags_38 |= 0x100;
    e->secondaryCollider.unk_0A = 0x10;
    e->params.unk_50 = 0xFFF6;
    e->scale = 0x40;
    e->sprite.unk_08[0] = 0x40;
    e->sprite.unk_08[1] = 0x40;
    e->unk_178 = params->unk_52;
    pool->mask |= 1 << slot;
    o->flags &= ~2;
    Sound_PlaySong(0x25A);
    return 0;
}
