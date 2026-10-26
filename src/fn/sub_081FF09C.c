// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct Pos {
    s16 x, y, z, pad;
};
struct Obj {
    u32 flags;
    u8 pad4[3];
    u8 priority;
    u8 pad8[8];
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
struct Params {
    u16 a, lifetime;
    u8 pad4[2];
    s8 angle;
    u8 pad7;
    u32 speed;
    u32 callback;
    s32 velX, velY, velZ;
    u8 pad1c[4];
    u32 resource;
    u32 frame;
    s32 bank;
    u32 script;
    u16 sequence;
    u8 pad32[2];
};
struct Entry {
    u32 timer;
    u8 pad4[0xc];
    s16 vx, vy, vz;
    u8 pad16[2];
    s32 x, y, z;
    u8 pad24[4];
    struct Anim anim;
    struct Params params;
    u8 animData[0x1c];
    struct Obj sprite;
    u32 scriptProc;
    u8 padtail[4];
};
struct Pool {
    u8 pad[0x18];
    u32 mask;
    struct Entry e[32];
};
extern const s16 gUnk_086149C4[];
void SpriteParticle_CreateManager(void);
struct Entry *SpriteParticle_FindFreeSlot(struct Pool *, s32 *);
void SpriteParticle_SetUpdateCallback(void *, void *);
void SpriteParticle_OnDefaultUpdate(void);
void Memory_Copy(void *, const void *, s32);
s32 Mod(s32, s32);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_LinkToList(void *, u32);
u32 ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void StepPlayback(struct Obj *sprite, struct Anim *state, u32 one)
{
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->frame << 1;
        const volatile u16 *table;
        table = state->data;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->timer + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r2");
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
s32 SpriteParticle_Emit(struct Params *params, struct Pos *pos)
{
    struct Pool *pool = gSpriteParticleManager;
    struct Entry *e;
    s32 slot;
    struct Obj *o;
    void *ad;
    struct Pos *v;
    s32 magnitude;
    s32 angle;
    u32 one;
    if (!pool) {
        SpriteParticle_CreateManager();
        return -1;
    }
    e = SpriteParticle_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    SpriteParticle_SetUpdateCallback(params, SpriteParticle_OnDefaultUpdate);
    Memory_Copy(&e->params, params, 0x34);
    e->timer = 0;
    v = (struct Pos *)&e->vx;
    angle = params->angle;
    magnitude = params->speed;
    {
        s32 scaled = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
        v->x = scaled;
    }
    v->y = 0;
    {
        s32 scaled = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
        v->z = scaled;
    }
    e->sprite.pos = *pos;
    e->x = pos->x << 12;
    e->y = pos->y << 12;
    e->z = pos->z << 12;
    if (Mod(e->timer, 10) == 0)
        e->params.velX >>= 1;
    if (Mod(e->timer, 20) == 0)
        e->params.velY >>= 1;
    ad = e->animData;
    AnimData_Load(ad, params->resource);
    o = &e->sprite;
    o->frame = params->frame;
    if (params->bank >= 0)
        AnimData_SelectPaletteBank(ad, params->bank);
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    {
        u8 *q = (u8 *)e + 0x8f;
        one = 1;
        asm("" : "+r"(one));
        *q = one;
    }
    if (e->params.script) {
        e->scriptProc = ScriptProc_Start(0x922E, (u16)e->params.script);
        Anim_SetSequence(&e->anim, (void *)e->scriptProc, e->params.sequence, 0, 0);
        StepPlayback(o, &e->anim, one);
    }
    pool->mask |= 1 << slot;
    return 0;
}
