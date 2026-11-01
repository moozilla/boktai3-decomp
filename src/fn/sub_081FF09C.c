#include "global.h"
#include "ram.h"
#include "player_cue_sequence.h"
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
void Sprite_LinkToList(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
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
        *q = one;
    }
    if (e->params.script) {
        e->scriptProc = (u32)ScriptProc_Start(0x922E, (u16)e->params.script);
        Anim_SetSequence(&e->anim, (void *)e->scriptProc, e->params.sequence, 0, 0);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)o,
                               (struct DisplaySequenceState *)&e->anim);
    }
    pool->mask |= 1 << slot;
    return 0;
}
