// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
#include "display_sequence_inline.h"
#include "projectile_pool.h"
extern u32 gUnk_02000560;
u8 *TerrainModifiers_FindCellOverride(int, int);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u32 FrameFactor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}
static inline void Polar(struct ContactPosition *d, struct ProjectileConfig *p)
{
    struct ContactPosition *dst = d;
    s32 angle = p->angle;
    s32 speed = p->speed;
    dst->x = Truncate(speed * gUnk_086149C4[(angle + 0x40) & 0xff]);
    dst->y = 0;
    dst->z = Truncate(speed * gUnk_086149C4[angle & 0xff]);
}
static inline u32 CommandFlip(u32 command, u32 axis)
{
    u32 flips = 0x30;
    flips &= command;
    return (flips >> 4) & axis;
}
static inline struct ProjectileSlot *Next(struct ProjectileSlot *e)
{
    return e + 1;
}
static inline u8 *CellData(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
s32 ProjectilePool_Update(struct TimedProjectilePool *pool)
{
    struct ProjectileSlot *cursor, *base;
    s32 i;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    base = pool->slots;
    for (i = 0, cursor = base; i <= 15; i++, cursor = Next(cursor), base = Next(base)) {
        u32 live = pool->mask & (1 << i);
        u16 *baseY = &base->params.baseY;
        if (live) {
            struct DisplaySequenceSprite *o = &base->sprite;
            struct ContactCollider *c = &base->collider;
            struct ProjectileConfig *p = &base->params;
            struct ContactPosition *v;
            struct DisplaySequenceSprite *spr;
            {
                s32 r = cursor->params.callback(base, i);
                spr = o;
                if (r == 1)
                    goto release;
            }
            asm("" ::"r"(o));
            {
                u32 move = cursor->flags & 1;
                v = &base->velocity;
                /* Nine uses raise v's allocation priority above cursor. */
                asm("" ::"r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v));
                if (move)
                    Polar(v, p);
            }
            {
                struct ContactPosition *pos = &o->position;
                o->position.x = o->position.x + cursor->velocity.x;
                pos->y = pos->y + v->y;
                pos->z = pos->z + v->z;
                p->velocity -= p->gravity;
                p->height += p->velocity;
                v = pos;
            }
            if (p->height < 0)
                o->position.y = *baseY - ((~p->height + 1) >> 12);
            else
                o->position.y = (p->height >> 12) + *baseY;
            if (o->position.y >= 0) {
                s32 x = o->position.x >> 8;
                s32 z = v->z >> 8;
                s32 index;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                if ((u32)((*CellData(index) & 0xf) << 8) <= o->position.y)
                    goto alive;
            }
            if (base->params.persist == 0) {
            release:
                ProjectilePool_ReleaseSlot(pool, base, i);
                continue;
            }
        alive:
            if (cursor->params.script) {
                struct DisplaySequenceSprite *s = spr;
                struct DisplaySequenceState *a = &base->anim;
                const u16 *entry = cursor->anim.commands + cursor->anim.index;

                s->frame = *entry >> 6;
                {
                    u32 input = cursor->anim.flags;
                    u32 left = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != CommandFlip(DisplaySequence_ReadCommand(entry), mask))
                        s->flags |= 4;
                    else
                        s->flags &= ~4;
                }
                {
                    u32 left = (u8)(a->flags & 2);
                    if (left != CommandFlip(DisplaySequence_ReadCommand(entry), 2))
                        s->flags |= 8;
                    else
                        s->flags &= ~8;
                }
                if (++a->elapsed >= a->duration) {
                    a->elapsed = 0;
                    if (a->flags & 4) {
                        if (a->index == 0) {
                            if (a->flags & 8)
                                a->elapsed = a->duration;
                            else
                                a->index = a->frameCount - 1;
                        } else {
                            a->index--;
                        }
                    } else {
                        if (++a->index >= a->frameCount) {
                            if ((u8)(a->flags & 8)) {
                                a->elapsed = a->duration;
                                a->index = a->frameCount - 1;
                            } else {
                                a->index = 0;
                            }
                        }
                    }
                    entry = a->commands + a->index;
                    a->frameWeight = FrameFactor(*entry);
                    a->duration = (a->frameWeight * a->speed) >> 6;
                    if (!a->duration)
                        a->duration = 1;
                }
            }
            if ((cursor->flags & 2) || (cursor->timer & 1)) {
                Collider_SetPosition(c, v, 0);
                Collider_Refresh(c);
            }
            cursor->timer++;
        }
    }
    return 0;
}
