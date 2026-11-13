#include "global.h"
#include "engine.h"
#include "ram.h"
#include "display_sequence_sprite.h"
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
    u32 pad0c;
    u32 callback;
    u32 resource;
    u32 frame;
    s32 bank;
    s32 gravity, vy, h;
    s16 y2c;
    s16 flag2e;
    u32 script;
    u32 sequence;
};
struct Entry {
    u32 timer;
    u32 flags4;
    u8 pad8[8];
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
extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void EnemyParticle_ReleaseSlot(struct Pool *, struct Entry *, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u8 *cell(s32 i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline void ComputeVelocity(struct Pos *v, struct Params *params)
{
    s32 angle = params->angle;
    s32 magnitude = params->speed;
    v->x = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    v->y = 0;
    v->z = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
}
s32 EnemyParticle_UpdatePool(struct Pool *pool)
{
    struct Entry *base, *cursor;
    s32 i;
    u32 pause = 0x400;
    if ((gEngineFlags | gScenePauseFlags) & pause)
        return 0;
    {
        u32 busy = 4;
        if (gUnk_02000560 & busy)
            return 0;
    }
    base = pool->e;
    for (i = 0, cursor = base; i <= 15; i++, cursor++, base++) {
        u32 bit = 1 << i;
        u32 live = pool->mask & bit;
        s16 *ground = &base->params.y2c;
        if (live) {
            struct Obj *o = &base->sprite;
            struct Params *params = &base->params;
            struct Pos *v;
            struct Pos *pos;
            s32 h;
            struct Obj *sprite;
            struct Pos *g;
            u32 recompute;
            s32 result;
            result = sub_08249240(base, (void *)i, (void *)cursor->params.callback);
            sprite = o;
            if (result == 1)
                goto release;
            recompute = cursor->flags4 & 1;
            v = (struct Pos *)&base->vx;
            if (recompute) {
                struct Pos *w = v;
                ComputeVelocity(w, params);
            }
            pos = &o->pos;
            o->pos.x += cursor->vx;
            pos->y += v->y;
            pos->z += v->z;
            params->vy -= params->gravity;
            h = params->h += params->vy;
            g = pos;
            if (h < 0)
                o->pos.y = *ground - ((~h + 1) >> 12);
            else
                o->pos.y = (h >> 12) + *ground;
            if (o->pos.y < 0)
                goto release;
            {
                s32 x = o->pos.x >> 8, z = g->z >> 8, index;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                if ((u32)((*cell(index) & 15) << 8) > o->pos.y) {
                release:
                    EnemyParticle_ReleaseSlot(pool, base, i);
                    continue;
                }
            }
            if (cursor->params.script)
                DisplaySequence_StepSprite((struct DisplaySequenceSprite *)&cursor->sprite,
                                           (struct DisplaySequenceState *)&cursor->anim);
            cursor->timer++;
        }
    }
    return 0;
}
