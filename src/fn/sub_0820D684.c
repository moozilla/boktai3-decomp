#include "global.h"
#include "player_cue_sequence.h"
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
    s8 mode;
    u32 speed;
    u32 callback;
    s32 velX;
    u32 resource;
    u32 frame;
    s32 bank;
    u8 pad20[0xc];
    s16 y2c;
    s16 flag2e;
    u32 script;
    u32 sequence;
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
    u8 padA8[4];
    u32 scriptProc;
};
struct Pool {
    u8 pad[0x18];
    u32 mask;
    struct Entry e[16];
};
extern const s16 gUnk_086149C4[];
void EnemyParticle_CreatePool(void);
struct Entry *EnemyParticle_FindFreeSlot(struct Pool *, s32 *);
void Memory_Copy(void *, const void *, s32);
s32 Mod(s32, s32);
void Sprite_LinkToList(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
s32 EnemyParticle_Emit(struct Params *params, struct Pos *pos)
{
    struct Pool *pool = gEnemyParticlePool;
    struct Entry *e;
    s32 slot;
    struct Obj *o;
    void *ad;
    struct Pos *v;
    s32 magnitude;
    s32 angle;
    if (!pool) {
        EnemyParticle_CreatePool();
        return -1;
    }
    e = EnemyParticle_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Memory_Copy(&e->params, params, 0x38);
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
    ad = e->animData;
    AnimData_Load(ad, params->resource);
    {
        struct Obj *s = &e->sprite;
        s32 bank;
        s->frame = params->frame;
        bank = params->bank;
        o = s;
        if (bank >= 0)
            AnimData_SelectPaletteBank(ad, bank);
    }
    e->sprite.flags &= ~1;
    e->sprite.flags |= params->a;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        e->scriptProc = ScriptProc_Start(0x922E, (u16)e->params.script);
        if (e->params.mode == 0)
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, 0, 0);
        else if (e->params.mode == 4) {
            u32 dir = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, dir, 0);
        } else {
            u32 dir = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->anim, (void *)e->scriptProc, (u16)e->params.sequence, dir, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)&e->sprite,
                               (struct DisplaySequenceState *)&e->anim);
    }
    e->params.y2c = e->sprite.pos.y;
    if (params->flag2e)
        e->sprite.flags |= 0x200;
    pool->mask |= 1 << slot;
    return 0;
}
