// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
struct ProjPos {
    s16 x, y, z, pad;
};
struct ProjSprite {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[10];
    struct ProjPos pos;
    u8 tail[8];
};
struct ProjAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct ProjParams {
    u16 h0, h2, bodyFlag;
    s8 angle;
    u8 pad7;
    s32 speed;
    u32 bodyId;
    u32 callback;
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
    struct ProjPos vel;
    struct ProjAnim anim;
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
extern u32 gUnk_030053F0;
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
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
static inline void Polar(struct ProjPos *d, struct ProjParams *p)
{
    register struct ProjPos *dst asm("r5") = d;
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
        if ((gEngineFlags | gUnk_030053F0) & mask)
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
            struct ProjPos *v;
            struct ProjSprite *spr;
            {
                s32 r = sub_08249240(base, (void *)i, (void *)cursor->params.callback);
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
                struct ProjPos *pos = &o->pos;
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
                register u32 dummy asm("r5");
                asm("" : "=r"(dummy));
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                asm("" ::"r"(dummy));
                if ((u32)((*CellData(index) & 0xf) << 8) <= o->pos.y)
                    goto alive;
            }
            if (((struct ProjParams *)((u8 *)baseY - 0x4c))->persist == 0) {
            release:
                ProjectilePool_ReleaseSlot(pool, base, i);
                continue;
            }
        alive:
            if (cursor->params.script) {
                register struct ProjSprite *s asm("r4") = spr;
                struct ProjAnim *a = &base->anim;
                const u16 *entry = cursor->anim.data + cursor->anim.frame;
                register u16 n asm("r0");
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
                n = ++a->timer;
                if (n >= a->rate) {
                    a->timer = 0;
                    if (a->flags & 4) {
                        if (a->frame == 0) {
                            if (a->flags & 8)
                                a->timer = a->rate;
                            else
                                a->frame = a->count - 1;
                        } else {
                            a->frame--;
                        }
                    } else {
                        if (++a->frame >= a->count) {
                            if ((u8)(a->flags & 8)) {
                                a->timer = a->rate;
                                a->frame = a->count - 1;
                            } else {
                                a->frame = 0;
                            }
                        }
                    }
                    entry = a->data + a->frame;
                    a->factor = FrameFactor(*entry);
                    a->rate = (a->factor * a->scale) >> 6;
                    if (!a->rate)
                        a->rate = 1;
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
