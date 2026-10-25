// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: per-frame update applies pending damage, starts the death sequence and steps the
// sprite animation.
#ifdef NONMATCHING
// fuzzy: 94.4%
// fix.py best (sweep after the inlined-insn fix)
// Remaining: collider flag-update registers and one copy of the constant 7 from sl.
struct DisplaySequenceState {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Col {
    u8 pad[6];
    u16 flags;
};
struct Spr {
    u32 flags;
    u8 pad[12];
    u16 frame;
};
struct Vec4 {
    u16 x, y, z, w;
};
struct Pair {
    s32 lo : 16;
    u32 hi : 16;
};
struct S {
    u8 p0[0x18];
    s32 timer;
    u8 p1c[0x24 - 0x1c];
    struct Spr spr;
    u8 p38[0x40 - 0x38];
    struct Vec4 pos;
    u8 p48[0x50 - 0x48];
    u8 anim[0x5c - 0x50];
    void *ptr5c;
    u8 p60[0x6c - 0x60];
    struct Col col;
    u8 p74[0x78 - 0x74];
    u8 dmgA[8];
    u8 dmgB[0xaf - 0x80];
    u8 direction;
    u16 invuln;
    u8 pb2[0x108 - 0xb2];
    void *fn;
    struct DisplaySequenceState seq;
    u8 p11c[0x126 - 0x11c];
    s16 hp;
    s16 damage;
    u16 hit;
    u8 p12c[2];
    s16 dead;
    s16 flash;
    u8 facing;
    u8 p133[5];
    s16 maxHp;
    u8 p13a[4];
    s16 hudStyle;
    u8 p140[2];
    s16 hudTime;
    u8 p144[0x150 - 0x144];
    u32 terrainEnabled;
    void *resource;
    u8 p158[4];
    void *owner;
    u32 hitFlags;
    u32 colliderHitFlags;
    u8 p168[0x174 - 0x168];
    u8 terrain[4];
};
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
extern u32 gUnk_02000560;
extern u8 *gUnk_030042E4;
extern u32 gUnk_03005308;
void AnimData_SelectPaletteBank(void *, u32 animation);
void Collider_SetPosition(void *collider, void *position, u32 value);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32 song);
void TerrainModifiers_UnregisterCell(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
void SceneHud_SetTimedState(void *, int, int, int, int, int);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void EnemyF426_UpdateDeath(void);
static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
inline static u32 TestFlag_UpdateDamageState(s16 mask, u32 *p)
{
    return (*p) & mask;
}

u32 EnemyF426_UpdateDamageState(struct S *p)
{
    struct Col *c;
    if (TestPair(0x400, &gUnk_030053F4, &gUnk_030053F0) ||
        TestFlag_UpdateDamageState(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->fn);
    p->spr.flags &= ~1;
    if (p->dead == 0) {
        c = &p->col;
        {
            s32 v1 = c->flags & (~4);
            c->flags = v1;
        }
        Collider_SetPosition(c, &p->pos, 0);
    } else {
        c = &p->col;
        c->flags |= 4;
    }
    {
        if ((p->damage > 0) && (!p->dead)) {
            p->hp -= p->damage;
            p->hitFlags = p->colliderHitFlags;
            Sound_PlaySong(0x13e);
            if (p->damage > 0) {
                DamageNumberDisplay_Show(c, p->damage, 0, &p->dmgA, &p->dmgB, 1);
                SceneHud_SetTimedState(p->owner, p->hp, p->maxHp, p->hudStyle, p->direction == 4,
                                       p->hudTime);
            }
            if (p->hp <= 0) {
                struct _PermuterAnon1 {
                    struct Pair a;
                    struct Pair b;
                } spread;
                struct _PermuterAnon1 velocity;
                struct Vec4 center;
                const u16 *table;
                u16 sample;
                register u32 r asm("r1");
                p->dead = 1;
                if (p->terrainEnabled)
                    TerrainModifiers_UnregisterCell(p->terrain);
                {
                    struct Vec4 *cp = &center;
                    *cp = p->pos;
                    cp->y += 0x12c;
                }
                velocity.a.lo = -4;
                table = (const u16 *)0x0203B400;
                r = (gUnk_03005308 + 1) & 0x3ff;
                sample = table[r];
                velocity.a.hi = sample & 7;
                velocity.b.lo = -4;
                spread.a.lo = 0x18;
                r = (r + 1) & 0x3ff;
                sample = table[r];
                spread.a.hi = (sample & 7) + 3;
                spread.b.lo = 0x18;
                r = (r + 1) & 0x3ff;
                gUnk_03005308 = r;
                sample = table[r];
                sub_08014634((sample & 3) + 4, 3, &center, &velocity, &spread, 15, 15);
                Anim_RestartSequence(&p->seq, p->resource, 1, p->facing, 0);
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                p->ptr5c = gUnk_030042E4 + 0x44a0;
                p->flash = 0;
                p->fn = EnemyF426_UpdateDeath;
                p->timer = 0;
                Sound_PlaySong(0x52a);
            }
            p->flash = 8;
            p->hit = 1;
            p->damage = 0;
        }
    }
    if ((p->flash > 0) && (p->dead == 0)) {
        AnimData_SelectPaletteBank(p->anim, 0x192);
        p->flash--;
    } else {
        AnimData_SelectPaletteBank(p->anim, 0x225);
    }
    if (p->invuln) {
        p->invuln--;
        if ((p->invuln >> 2) & 1)
            p->spr.flags |= 1;
        else
            p->spr.flags &= ~1;
    }
    {
        struct Spr *sp = &p->spr;
        struct DisplaySequenceState *s = &p->seq;
        const u16 *c = s->commands + s->index;
        sp->frame = (*c) >> 6;
        {
            u32 flags = s->flags;
            u32 bit = 1;
            u32 lhs = bit;
            asm("" : "+r"(lhs));
            lhs &= flags;
            if (lhs != ((((*c) & 0x30) >> 4) & bit))
                p->spr.flags |= 4;
            else
                p->spr.flags &= ~4;
        }
        {
            u32 flags = s->flags;
            u32 bit = 2;
            u32 lhs = bit;
            asm("" : "+r"(lhs));
            lhs &= flags;
            asm("" : "+r"(lhs));
            if (((u8)lhs) != ((((*c) & 0x30) >> 4) & bit))
                sp->flags |= 8;
            else
                sp->flags &= ~8;
        }
        {
            u32 elapsed = s->elapsed + 1;
            u32 mask;
            u32 duration;
            register u32 saved asm("r4");
            s->elapsed = elapsed;
            {
                register u32 temp asm("r2") = 0xffff;
                asm("" : "+r"(temp));
                mask = temp;
                asm("" : "+&r"(mask) : "r"(temp));
            }
            duration = s->duration;
            elapsed = (u16)elapsed;
            saved = duration;
            if (elapsed >= duration) {
                u32 flags;
                u32 idx;
                s->elapsed = 0;
                flags = s->flags;
                if (flags & 4) {
                    idx = s->index;
                    if (idx == 0) {
                        u32 hold = 8;
                        hold &= flags;
                        if (hold) {
                            s->elapsed = saved;
                            goto frame_ready;
                        }
                        idx = s->frameCount;
                    }
                    idx--;
                } else {
                    u32 count;
                    idx = s->index + 1;
                    s->index = idx;
                    count = s->frameCount;
                    if ((idx & mask) < count)
                        goto frame_ready;
                    idx = 8;
                    idx &= flags;
                    idx = (u8)idx;
                    if (idx) {
                        s->elapsed = duration;
                        idx = count - 1;
                    }
                }
                s->index = idx;
            frame_ready:
                c = s->commands + s->index;

                s->frameWeight = (*((const volatile u16 *)c)) & 15;
                s->duration = (s->frameWeight * s->speed) >> 6;
                if (s->duration == 0)
                    s->duration = 1;
            }
        }
    }
    p->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", EnemyF426_UpdateDamageState);
#endif
