// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: parses the spawn script (position, attack, health, timing keywords), loads
// graphics and the first animation frame, and installs the idle callback.
#include "scene_vec.h"
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
extern struct PHTerrain *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void EnemyF426_InitCollider(void *);
void sub_08203CA0(void);
s32 Script_GetValueSafe(void);
void EnemyF426_ReadScriptValueList(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 EnemyF426_CellIndex(struct PHVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline void EnemyF426_StepAnimation(u8 *p, void *sprite, struct FanSeq *s)
{
    u32 flags, elapsed;
    u32 duration;
    // Reuse the command address temporary as the index wrap mask.
    u32 commandOrMask;
    register u32 savedDuration asm("r4");
    commandOrMask = (u32)(s->commands + s->index);
    {
        u32 frame;
        register void *highSprite asm("r8") = sprite;
        void *frameSprite;
        frame = *(const volatile u16 *)commandOrMask >> 6;
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
        if (first != (((*(const volatile u16 *)commandOrMask & 0x30) >> 4) & bit))
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
        if (first != (((*(const volatile u16 *)commandOrMask & 0x30) >> 4) & bit)) {
            register u32 *f asm("r3") = sprite;
            *f |= 8;
        } else {
            register u32 *f asm("r4") = sprite;
            *f &= ~8;
        }
    }
    elapsed = s->elapsed + 1;
    s->elapsed = elapsed;
    {
        register u32 temp asm("r2") = 0xffff;
        asm("" : "+r"(temp));
        commandOrMask = temp;
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
            if ((i & commandOrMask) >= n) {
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
        commandOrMask = (u32)(s->commands + s->index);
        s->weight = *(const volatile u16 *)commandOrMask & 15;
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
s32 EnemyF426_InitializeFromScript(u8 *p, void *parent)
{
    register void *sprite asm("r8");
    void *earlySprite;
    void **slot;
    void *resource;
    s32 found;
    s32 dead;
    if (Script_SeekToKeyword('p')) {
        H(0x1c) = Script_GetValue();
        H(0x1e) = Script_GetValue();
        H(0x20) = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        H(0x120) = Script_GetValue();
        H(0x122) = Script_GetValue();
        H(0x124) = Script_GetValue();
        H(0x136) = Script_GetValue();
    } else {
        H(0x120) = 30;
        H(0x122) = 30;
        H(0x124) = 30;
        H(0x136) = 24;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        H(0x138) = health;
        H(0x126) = health;
        if ((s16)health < 0)
            H(0x144) = 1;
    } else {
        H(0x138) = 120;
        H(0x126) = 120;
    }
    if (Script_SeekToKeyword('d'))
        W(0x150) = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        H(0x134) = Script_GetValue();
    else
        H(0x134) = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *b = (s8 *)(p + 0x132);
        *b = v;
        *b = (*b - 1) / 2;
        if ((u8)*b > 3)
            *b = 1;
    } else
        *(u8 *)(p + 0x132) = 1;
    if (Script_SeekToKeyword('r'))
        W(0x148) = Script_GetValueSafe();
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
        W(0x14c) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x168) = Script_GetValue();
        W(0x16c) = Script_GetValue();
        W(0x170) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        H(0x158) = Script_GetValue();
        H(0x15a) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        H(0x142) = Script_GetValue();
        if (H(0x142) > 99)
            H(0x142) = 99;
        if (H(0x142) <= 0)
            H(0x142) = 1;
    } else
        H(0x142) = 0;
    *(void **)(p + 0x15c) = parent;
    AnimData_Load(p + 0x50, 0x7e82);
    earlySprite = p + 0x24;
    Sprite_Init(earlySprite, p + 0x50, 1);
    *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
    AnimData_SelectPaletteBank(p + 0x50, 0x225);
    resource = ScriptProc_Start(0x922e, 0xfa45);
    slot = (void **)(p + 0x154);
    *slot = resource;
    dead = H(0x144);
    sprite = earlySprite;
    if (dead == 0) {
        Anim_SetSequence(p + 0x10c, resource, 0, *(u8 *)(p + 0x132), dead);
    } else {
        register u32 zero;
        s16 *q = &H(0x12e);
        s32 four = 4;
        zero = 0;
        *q = 1;
        asm("" : : : "memory");
        Anim_SetSequence(p + 0x10c, *slot, 1, *(u8 *)((u8 *)q + four), zero);
    }
    EnemyF426_StepAnimation(p, sprite, (struct FanSeq *)(p + 0x10c));
    {
        register u32 offset asm("r3") = 0x144;
        s16 *deadPtr;
        asm("" : "+r"(offset));
        deadPtr = (s16 *)(p + offset);
        if (*deadPtr == 0)
            EnemyF426_InitCollider(p);
        if (W(0x150) && *deadPtr == 0) {
            s32 cell = EnemyF426_CellIndex((struct PHVec *)(p + 0x40));
            TerrainModifiers_RegisterCell(p + 0x174, cell, 0, H(0x42) >> 8, 0, 1);
        }
    }
    if (H(0x140)) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *(void **)(p + 0x15c), p + 0x40, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (W(0x148))
        EnemyF426_ReadScriptValueList(p);
    *(void (**)(void))(p + 0x108) = sub_08203CA0;
}
