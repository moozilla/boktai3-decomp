#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.6%
// foundry draft (worker best 91.3%); what differs is mostly register allocation and instruction order
struct DisplaySequenceState {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Col { u8 pad[6]; u16 flags; };
struct Spr { u32 flags; u8 pad[12]; u16 frame; };
struct S {
    u8 p0[0x18];
    s32 timer;
    u8 p1c[0x24 - 0x1c];
    struct Spr spr;
    u8 p36[0x40 - 0x38 + 0];
    u8 pos[0x10];
    u8 anim[0x5c - 0x50];
    void *ptr5c;
    u8 p60[0xc0 - 0x60];
    struct Col col;
    u8 pc8[0xcc - 0xc8];
    u8 dmgA[8];
    u8 dmgB[0x100 - 0xd4];
    u8 pad100[3];
    u8 b103;
    u16 invuln;
    u8 p106[0x15c - 0x106];
    void *fn15c;
    struct DisplaySequenceState seq;
    u8 p170[0x17c - 0x170];
    u16 hp;
    u8 p17e[2];
    s16 damage;
    u16 w182;
    u8 p184[2];
    s16 dead;
    u8 p188[0x18c - 0x188];
    s16 timer18c;
    u8 p18e[2];
    s16 w190;
    s16 w192;
    s16 w194;
    s16 w196;
    s16 w198;
    u8 p19a[4];
    s16 w19e;
    u8 p1a0[2];
    s16 w1a2;
    u8 p1a4[0x1ac - 0x1a4];
    void *w1ac;
    u8 p1b0[4];
    u32 w1b4;
    u32 w1b8;
    u8 p1bc[0x1c8 - 0x1bc];
    u32 w1c8;
    u32 w1cc;
    u8 w1d0[4];
};
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
extern u32 gUnk_02000560;
extern u8 *gUnk_030042E4;
void AnimData_SelectPaletteBank(void *, u32 animation);
void Collider_SetPosition(void *collider, void *position, u32 value);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32 song);
void TerrainModifiers_UnregisterCell(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
void SceneHud_SetTimedState(void *, int, int, int, int, int);
u32 ScriptCombatActor_UpdateDefeatEffect(void *);
u32 sub_08201D38(struct S *p)
{
    struct Col *c;
    u32 m = 0x400;
    if ((gUnk_030053F4 | gUnk_030053F0) & m)
        return 0;
    {
        u32 m2 = 4;
        if (gUnk_02000560 & m2)
            return 0;
    }
    Callback_InvokeActor(p, p->fn15c);
    p->spr.flags &= ~1;
    if (p->dead == 0) {
        c = &p->col;
        c->flags &= ~4;
        Collider_SetPosition(c, p->pos, 0);
    } else {
        c = &p->col;
        c->flags |= 4;
        p->ptr5c = gUnk_030042E4 + 0x36e0;
    }
    {
        u32 off = 0x180;
        s16 *dp = (s16 *)((u8 *)p + off);
        u16 d = *dp;
        if (*dp > 0 && *(s16 *)((u8 *)p + off + 6) == 0) {
            u16 *hpp = &p->hp;
            *hpp -= d;
            p->w1b4 = p->w1b8;
            Sound_PlaySong(0x13e);
            if (*dp > 0) {
                u32 off2 = 0x19e;
                DamageNumberDisplay_Show(c, *dp, 0, &p->dmgA, &p->dmgB, 1);
                SceneHud_SetTimedState(p->w1ac, (s16)*hpp, p->w198, *(s16 *)((u8 *)p + off2),
                             *((u8 *)p + off2 - 0x9b) == 4, p->w1a2);
            }
            if ((s16)p->hp <= 0) {
                p->dead = 1;
                if (p->w1c8)
                    TerrainModifiers_UnregisterCell(&p->w1d0);
                Anim_RestartSequence(&p->seq, (void *)p->w1cc, 2, 0, 0);
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                p->ptr5c = gUnk_030042E4 + 0x36e0;
                p->timer18c = 0;
                p->timer = 0;
                p->fn15c = ScriptCombatActor_UpdateDefeatEffect;
                Sound_PlaySong(0x529);
            }
            p->timer18c = 8;
            p->w182 = 1;
            p->damage = 0;
        }
    }
    if (p->timer18c > 0 && p->dead == 0) {
        AnimData_SelectPaletteBank(p->anim, 0x192);
        p->timer18c--;
    } else {
        AnimData_SelectPaletteBank(p->anim, 0x1b7);
        if (p->dead == 0) {
            if (p->w190 == 0) {
                AnimData_SelectPaletteBank(p->anim, 0x1b8);
                if (p->w196 > p->w194) {
                    p->w190 = 1;
                    p->w196 = 0;
                }
            } else {
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                if (p->w196 > p->w192) {
                    p->w190 = 0;
                    p->w196 = 0;
                }
            }
        }
        p->w196++;
        if (p->invuln) {
            p->invuln--;
            if ((p->invuln >> 2) & 1)
                p->spr.flags |= 1;
            else
                p->spr.flags &= ~1;
        }
    }
    {
        struct DisplaySequenceState *s = &p->seq;
        struct Spr *sp = &p->spr;
        const u16 *c = s->commands + s->index;
        sp->frame = *c >> 6;
        if ((s->flags & 1) != (((*c & 0x30) >> 4) & 1))
            sp->flags |= 4;
        else
            sp->flags &= ~4;
        if ((u8)(s->flags & 2) != (((*c & 0x30) >> 4) & 2))
            sp->flags |= 8;
        else
            sp->flags &= ~8;
        s->elapsed++;
        if (s->elapsed >= s->duration) {
            s->elapsed = 0;
            if (s->flags & 4) {
                if (s->index == 0) {
                    if (s->flags & 8) {
                        s->elapsed = s->duration;
                        goto frame_ready;
                    }
                    s->index = s->frameCount;
                }
                s->index--;
            } else {
                s->index++;
                if (s->index >= s->frameCount) {
                    if (s->flags & 8) {
                        s->elapsed = s->duration;
                        s->index = s->frameCount - 1;
                    } else
                        s->index = 0;
                }
            }
        frame_ready:
            c = s->commands + s->index;
            s->frameWeight = *c & 15;
            s->duration = (s->speed * s->frameWeight) >> 6;
            if (s->duration == 0)
                s->duration = 1;
        }
    }
    p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08201D38);
#endif
