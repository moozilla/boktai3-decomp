#include "global.h"
#include "render.h"
#include "ram.h"
#include "engine_globals.h"
/* Tentative species identity: applies damage, starts defeat effects, and advances animation. */
#include "display_sequence_inline.h"
struct EnemyF426Collider {
    u8 pad[6];
    u16 flags;
};
struct EnemyF426Sprite {
    u32 flags;
    u8 pad[12];
    u16 frame;
};
struct EnemyF426Position {
    u16 x, y, z, w;
};
struct EnemyF426EffectPair {
    s32 lo : 16;
    u32 hi : 16;
};
struct EnemyF426Actor {
    u8 p0[0x18];
    s32 timer;
    u8 p1c[0x24 - 0x1c];
    struct EnemyF426Sprite spr;
    u8 p38[0x40 - 0x38];
    struct EnemyF426Position pos;
    u8 p48[0x50 - 0x48];
    u8 anim[0x5c - 0x50];
    void *ptr5c;
    u8 p60[0x6c - 0x60];
    struct EnemyF426Collider col;
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
extern u32 gUnk_02000560;
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
static inline u32 EnemyF426_TestEitherFlags(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 EnemyF426_TestFlag(u32 mask, u32 *p)
{
    return (*p) & mask;
}

static inline u32 EnemyF426_NextSample(const u16 *samples)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return samples[gRandomIndex];
}

static inline u32 EnemyF426_StepSequence(struct EnemyF426Sprite *owner,
                                         struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

u32 EnemyF426_UpdateDamageState(struct EnemyF426Actor *p)
{
    struct EnemyF426Collider *collider;
    if (EnemyF426_TestEitherFlags(0x400, &gEngineFlags, &gScenePauseFlags) ||
        EnemyF426_TestFlag(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->fn);
    p->spr.flags &= ~1;
    if (p->dead == 0) {
        struct EnemyF426Collider *c = &p->col;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->pos, 0);
        collider = c;
    } else {
        struct EnemyF426Collider *c = &p->col;
        c->flags |= 4;
        collider = c;
    }
    {
        if ((p->damage > 0) && (!p->dead)) {
            p->hp -= p->damage;
            p->hitFlags = p->colliderHitFlags;
            Sound_PlaySong(0x13e);
            if (p->damage > 0) {
                DamageNumberDisplay_Show(collider, p->damage, 0, &p->dmgA, &p->dmgB, 1);
                SceneHud_SetTimedState(p->owner, p->hp, p->maxHp, p->hudStyle, p->direction == 4,
                                       p->hudTime);
            }
            if (p->hp <= 0) {
                struct EnemyF426EffectVector {
                    struct EnemyF426EffectPair a;
                    struct EnemyF426EffectPair b;
                } spread;
                struct EnemyF426EffectVector velocity;
                struct EnemyF426Position center;
                const u16 *table;
                u32 sample;
                p->dead = 1;
                if (p->terrainEnabled)
                    TerrainModifiers_UnregisterCell(p->terrain);
                {
                    struct EnemyF426Position *cp = &center;
                    *cp = p->pos;
                    cp->y += 0x12c;
                }
                velocity.a.lo = -4;
                table = gRandomTable;
                sample = EnemyF426_NextSample(table);
                velocity.a.hi = sample & 7;
                velocity.b.lo = -4;
                spread.a.lo = 0x18;
                sample = EnemyF426_NextSample(table);
                spread.a.hi = (sample & 7) + 3;
                spread.b.lo = 0x18;
                sample = EnemyF426_NextSample(table);
                sub_08014634((sample & 3) + 4, 3, &center, &velocity, &spread, 15, 15);
                Anim_RestartSequence(&p->seq, p->resource, 1, p->facing, 0);
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                p->ptr5c = gPaletteBankData + 0x44a0;
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
    EnemyF426_StepSequence((void *)&p->spr, &p->seq);
    p->timer++;
    /* The callback has no explicit return-value assignment on this path in the ROM. */
}
