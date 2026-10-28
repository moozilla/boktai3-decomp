// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_vec.h"
#include "engine_globals.h"
struct PHVec {
    s16 x, y, z, pad;
};
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct PHTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void PoisonHand_InitCollider(void *);
u32 PoisonHand_WaitForPlayerApproach(u8 *);
void PoisonHand_UpdateDefeatBurst(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 PoisonHand_CellIndex(struct PHVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct PHTerrain *)gTerrainMap)->rows[z] + x;
}
static inline void PoisonHand_StepAnimation(u8 *p, void *sprite, struct FanSeq *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r7");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    {
        u32 frame;
        register void *highSprite asm("r8") = sprite;
        void *frameSprite;
        frame = *c >> 6;
        frameSprite = highSprite;
        asm("" : "+r"(frameSprite));
        *(u16 *)((u8 *)frameSprite + 0x10) = frame;
    }
    {
        u32 input = s->flags, bit = 1;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*c & 0x30) >> 4) & bit))
            *(u32 *)(p + 0x24) |= 4;
        else
            *(u32 *)(p + 0x24) &= ~4;
    }
    {
        u32 input = s->flags, bit = 2;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*c & 0x30) >> 4) & bit))
            *(u32 *)sprite |= 8;
        else
            *(u32 *)sprite &= ~8;
    }
    elapsed = s->elapsed + 1;
    s->elapsed = elapsed;
    {
        register u32 temp asm("r2") = 0xffff;
        asm("" : "+r"(temp));
        mask = temp;
    }
    duration = s->duration;
    elapsed = (u16)elapsed;
    savedDuration = duration;
    asm("" : "+r"(savedDuration) : "r"(elapsed));
    if ((u16)elapsed >= duration) {
        s->elapsed = 0;
        flags = s->flags;
        if (flags & 4) {
            u32 i = s->index;
            if (!i) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        s->elapsed = savedDuration;
                    else
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
            if ((i & mask) >= n) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        s->elapsed = duration;
                        s->index = n - 1;
                    } else
                        s->index = 0;
                }
            }
        }
        c = s->commands + s->index;
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
s32 PoisonHand_InitializeFromScript(u8 *p, void *parent)
{
    void *resource;
    register void *sprite asm("r8");
    void *earlySprite;
    register void **slot asm("r4");
    s32 found;
    s32 dead;
    if (Script_SeekToKeyword('p')) {
        H(0x1c) = Script_GetValue();
        H(0x1e) = Script_GetValue();
        H(0x20) = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        W(0x11c) = r * r;
    } else
        W(0x11c) = 0x40000;
    found = Script_SeekToKeyword('a');
    if (found) {
        H(0x120) = Script_GetValue();
        H(0x122) = Script_GetValue();
        H(0x124) = Script_GetValue();
    } else {
        H(0x120) = found;
        H(0x122) = found;
        H(0x124) = 30;
    }
    if (Script_SeekToKeyword('s'))
        H(0x12c) = Script_GetValue();
    else
        H(0x12c) = 2;
    if (Script_SeekToKeyword('d'))
        H(0x12e) = Script_GetValue();
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        H(0x138) = health;
        H(0x136) = health;
        if ((s16)health < 0)
            H(0x144) = 1;
    } else {
        H(0x138) = 120;
        H(0x136) = 120;
    }
    if (Script_SeekToKeyword('S'))
        H(0x140) = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        H(0x13a) = Script_GetValue();
        H(0x13c) = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        H(0x13e) = Script_GetValue();
    else
        H(0x13e) = 2;
    if (Script_SeekToKeyword('D'))
        W(0x164) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x158) = Script_GetValue();
        W(0x15c) = Script_GetValue();
        W(0x160) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        H(0x148) = Script_GetValue();
        H(0x14a) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        H(0x142) = Script_GetValue();
        if (H(0x142) > 99)
            H(0x142) = 99;
        if (H(0x142) <= 0)
            H(0x142) = 1;
    } else
        H(0x142) = 0;
    *(void **)(p + 0x14c) = parent;
    AnimData_Load(p + 0x50, 0x7e7d);
    earlySprite = p + 0x24;
    Sprite_Init(earlySprite, p + 0x50, 1);
    *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
    AnimData_SelectPaletteBank(p + 0x50, 0x1c5);
    resource = ScriptProc_Start(0x922e, 0x5184);
    slot = (void **)(p + 0x168);
    *slot = resource;
    dead = H(0x144);
    sprite = earlySprite;
    if (dead == 0) {
        *(void (**)(void))(p + 0x108) = (void (*)(void))PoisonHand_WaitForPlayerApproach;
        Anim_SetSequence(p + 0x10c, resource, 0x17, 0, dead);
    } else {
        *(void (**)(void))(p + 0x108) = PoisonHand_UpdateDefeatBurst;
        {
            u32 zero;
            s16 *q = &H(0x132);
            zero = 0;
            *q = 1;
            Anim_SetSequence(p + 0x10c, ({
                                 void *r = *slot;
                                 r;
                             }),
                             0x19, 0, zero);
        }
    }
    PoisonHand_StepAnimation(p, sprite, (struct FanSeq *)(p + 0x10c));
    if (H(0x144) == 0)
        PoisonHand_InitCollider(p);
    if (H(0x12e) && H(0x144) == 0) {
        s32 cell = PoisonHand_CellIndex((struct PHVec *)(p + 0x40));
        TerrainModifiers_RegisterCell(p + 0x16c, cell, 0, H(0x42) >> 8, 0, 1);
    }
    if (H(0x140)) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *(void **)(p + 0x14c), p + 0x40, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
}
