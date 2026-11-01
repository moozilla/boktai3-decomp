#include "global.h"
#include "player_cue_sequence.h"
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
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetPosition(void *, void *, u32);
void Sprite_LinkToList(void *, u32);
u8 *SceneBody_GetById(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
s32 Projectile_SpawnFromConfig(struct Params *params, struct Pos *pos)
{
    struct Pool *pool = gProjectilePool;
    struct Entry *e;
    s32 slot;
    u8 *q;
    s32 magnitude;
    s32 angle;
    u32 zero;
    struct Obj *o;
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
        o = t;
        if (bank >= 0)
            AnimData_SelectPaletteBank(q, bank);
    }
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        e->scriptProc = (u32)ScriptProc_Start(0x922E, (u16)e->params.script);
        if (e->params.variants == 0) {
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, 0, zero);
        } else if (e->params.variants == 4) {
            u32 var = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, var, zero);
        } else {
            u32 var = (((u16)e->params.angle + 0x70) >> 5) & 7;
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, var, zero);
        }
        {
            struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)o;
            PlayerCue_StepSequence(sprite, (struct DisplaySequenceState *)&e->anim);
        }
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
