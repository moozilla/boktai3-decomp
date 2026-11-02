// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
#include "display_sequence_inline.h"
#include "scene_vec.h"
struct ProjSprite {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[10];
    struct SceneVec pos;
    u8 tail[8];
};
struct ProjEntry;
typedef s32 (*ProjectileSlotUpdate)(struct ProjEntry *, s32);
struct ProjParams {
    u16 h0, h2, bodyFlag;
    s8 angle;
    u8 pad7;
    s32 speed;
    u32 bodyId;
    ProjectileSlotUpdate callback;
    u8 pad14[0x2c];
    s32 gravity;
    s32 velocity;
    s32 height;
    u16 baseY;
    s16 h4e;
    u32 script;
    u32 sequence;
    s16 persist;
    u8 pad5a[2];
};
struct ProjEntry {
    u32 timer;
    u32 flags;
    u8 pad8[8];
    struct SceneVec vel;
    struct DisplaySequenceState anim;
    struct ProjParams params;
    u8 animData[0x1c];
    struct ProjSprite sprite;
    u8 collider[0x54];
    u32 body;
    u32 scriptProc;
};
struct ProjPool {
    u8 pad[0x18];
    u32 mask;
    struct ProjEntry e[16];
};
extern u32 gUnk_02000560;
void ProjectilePool_ReleaseSlot(struct ProjPool *, struct ProjEntry *, s32);
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
static inline void Polar(struct SceneVec *d, struct ProjParams *p)
{
    struct SceneVec *dst = d;
    s32 angle = p->angle;
    s32 speed = p->speed;
    dst->x = Truncate(speed * gUnk_086149C4[(angle + 0x40) & 0xff]);
    dst->y = 0;
    dst->z = Truncate(speed * gUnk_086149C4[angle & 0xff]);
}
static inline u32 FlipBits(u32 c)
{
    u32 x = (0x30 & c) >> 4;
    return x;
}
static inline struct ProjEntry *Next(struct ProjEntry *e)
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
s32 ProjectilePool_Update(struct ProjPool *pool)
{
    struct ProjEntry *cursor, *base;
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
    base = pool->e;
    for (i = 0, cursor = base; i <= 15; i++, cursor = Next(cursor), base = Next(base)) {
        u32 live = pool->mask & (1 << i);
        u16 *baseY = &base->params.baseY;
        if (live) {
            struct ProjSprite *o = &base->sprite;
            void *c = base->collider;
            struct ProjParams *p = &base->params;
            struct SceneVec *v;
            struct ProjSprite *spr;
            {
                s32 r = cursor->params.callback(base, i);
                spr = o;
                if (r == 1)
                    goto release;
            }
            asm("" ::"r"(o));
            {
                u32 move = cursor->flags & 1;
                v = &base->vel;
                /* Nine uses raise v's allocation priority above cursor. */
                asm("" ::"r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v), "r"(v));
                if (move)
                    Polar(v, p);
            }
            {
                struct SceneVec *pos = &o->pos;
                o->pos.x = o->pos.x + cursor->vel.x;
                pos->y = pos->y + v->y;
                pos->z = pos->z + v->z;
                p->velocity -= p->gravity;
                p->height += p->velocity;
                v = pos;
            }
            if (p->height < 0)
                o->pos.y = *baseY - ((~p->height + 1) >> 12);
            else
                o->pos.y = (p->height >> 12) + *baseY;
            if (o->pos.y >= 0) {
                s32 x = o->pos.x >> 8;
                s32 z = v->z >> 8;
                s32 index;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                if ((u32)((*CellData(index) & 0xf) << 8) <= o->pos.y)
                    goto alive;
            }
            if (base->params.persist == 0) {
            release:
                ProjectilePool_ReleaseSlot(pool, base, i);
                continue;
            }
        alive:
            if (cursor->params.script) {
                struct ProjSprite *s = spr;
                struct DisplaySequenceState *a = &base->anim;
                const u16 *entry = cursor->anim.commands + cursor->anim.index;

                s->frame = *entry >> 6;
                {
                    u32 input = cursor->anim.flags;
                    u32 left = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != (((*entry & 0x30) >> 4) & mask))
                        s->flags |= 4;
                    else
                        s->flags &= ~4;
                }
                {
                    u32 left = (u8)(a->flags & 2);
                    register u32 cmd asm("r2") = *entry;
                    asm("" : "+r"(cmd));
                    if (left != (FlipBits(cmd) & 2))
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
