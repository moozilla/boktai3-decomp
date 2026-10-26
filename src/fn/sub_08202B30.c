// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct Pos {
    s16 x, y, z, pad;
};
struct Obj {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[10];
    struct Pos pos;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Pair {
    s32 a, b;
};
struct Params {
    u16 h0, h2, bodyFlag;
    s8 angle;
    u8 pad7;
    u32 speed;
    u32 bodyId;
    u32 callback;
    u32 w14, w18;
    s16 h1c, h1e, variants, h22;
    struct Pair extent;
    struct Pair offset;
    u32 resource;
    u32 frame;
    s32 bank;
    u8 pad40[0xc];
    u16 h4c;
    s16 h4e;
    u32 script;
    u32 sequence;
    u8 pad58[4];
};
struct Collider {
    u8 pad[0x14];
    struct Pair offset;
    struct Pair extent;
    u8 tail[0x54 - 0x24];
};
struct Entry {
    u32 timer;
    u8 pad4[0xc];
    s16 vx, vy, vz;
    u8 pad16[2];
    struct Anim anim;
    struct Params params;
    u8 animData[0x1c];
    struct Obj sprite;
    struct Collider collider;
    u32 body;
    u32 scriptProc;
};
struct Pool {
    u8 pad[0x18];
    u32 mask;
    struct Entry e[16];
};
extern const s16 gUnk_086149C4[];
struct Pool *ProjectilePool_Create(void);
struct Entry *ProjectilePool_FindFreeSlot(struct Pool *, s32 *);
void Memory_Copy(void *, const void *, s32);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetPosition(void *, void *, u32);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_LinkToList(void *, u32);
u32 ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
u8 *SceneBody_GetById(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
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
        if (first != (((*command & 0x30) >> 4) & bit))
            copy->flags |= 8;
        else
            copy->flags &= ~8;
    }
    {
        u32 elapsed = state->timer + 1;
        register u32 mask asm("r5");
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
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
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
#define o ((struct Obj *)pos)
s32 Projectile_SpawnFromConfig(struct Params *params, struct Pos *pos)
{
    struct Pool *pool = gProjectilePool;
    struct Entry *e;
    s32 slot;
    u8 *q;
    s32 magnitude;
    s32 angle;
    u32 zero;
    if (!pool) {
        pool = ProjectilePool_Create();
        if (!pool)
            return -1;
    }
    e = ProjectilePool_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Memory_Copy(&e->params, params, 0x5c);
    e->timer = 0;
    q = (u8 *)&e->vx;
    angle = params->angle;
    magnitude = params->speed;
    ((struct Pos *)q)->x = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    ((struct Pos *)q)->y = 0;
    ((struct Pos *)q)->z = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
    zero = 0;
    q = (u8 *)&e->collider;
    Collider_SetAttackProperties(q, params->h1c, params->h1e, params->w14, params->w18,
                                 params->h22);
    ((struct Collider *)q)->extent = params->extent;
    ((struct Collider *)q)->offset = params->offset;
    Collider_SetPosition(q, pos, 0);
    e->sprite.pos = *pos;
    q = e->animData;
    AnimData_Load(q, params->resource);
    {
        struct Obj *t = &e->sprite;
        s32 bank;
        t->frame = params->frame;
        bank = params->bank;
        pos = (struct Pos *)t;
        if (bank >= 0)
            AnimData_SelectPaletteBank(q, bank);
    }
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        e->scriptProc = ScriptProc_Start(0x922E, (u16)e->params.script);
        if (e->params.variants == 0) {
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, (void *)e->scriptProc, (u16)e->params.sequence, 0, zero);
        } else if (e->params.variants == 4) {
            u32 var = (((u16)e->params.angle + 0x60) >> 6) & 3;
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, (void *)e->scriptProc, (u16)e->params.sequence, var, zero);
        } else {
            u32 var = (((u16)e->params.angle + 0x70) >> 5) & 7;
            q = (u8 *)&e->anim;
            Anim_SetSequence(q, (void *)e->scriptProc, (u16)e->params.sequence, var, zero);
        }
        StepPlayback(e, o, (struct Anim *)q);
    }
    if (params->bodyFlag) {
        u8 *body = SceneBody_GetById(params->bodyId);
        if (body)
            e->body = (u32)(body + 0xc);
    }
    e->params.h4c = e->sprite.pos.y;
    if (params->h4e)
        o->flags |= 0x200;
    pool->mask |= 1 << slot;
    return 0;
}
