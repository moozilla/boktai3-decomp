// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
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
            if (cursor->params.script) {
                struct Obj *s = sprite;
                struct Anim *state = &base->anim;
                const volatile u16 *command = cursor->anim.data + cursor->anim.frame;
                u32 duration, flags;
                asm("" : "+r"(state));
                s->frame = *command >> 6;
                {
                    u32 input = cursor->anim.flags;
                    u32 left = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != (((*command & 0x30) >> 4) & mask))
                        cursor->sprite.flags |= 4;
                    else
                        cursor->sprite.flags &= ~4;
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
                        s->flags |= 8;
                    else
                        s->flags &= ~8;
                }
                {
                    u32 elapsed = state->timer + 1;
                    u32 mask;
                    register u32 temp asm("r2");
                    register u32 savedDuration asm("r2");
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
            cursor->timer++;
        }
    }
    return 0;
}
