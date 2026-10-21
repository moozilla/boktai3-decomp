// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.2%
// Register choice for the z pointer (r7 in the ROM), the facing-index pointer
// reloads and the Anim_SetSequence argument setup still differ.
#include "scene_vec.h"
#define EnemyD459_InitializeFromScript sub_0820539C
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
struct SpawnOffset {
    s32 x, z;
};
struct SpawnOffsets {
    struct SpawnOffset e[4];
};
extern const struct SpawnOffsets gUnk_0824FE5C;
extern struct PHTerrain *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void EnemyD459_InitCollider(u8 *);
void EnemyD459_ReadScriptValueList(u8 *);
s32 EnemyD459_UpdateAttackTiming(void);
s32 EnemyD459_UpdateDeath(u8 *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(s8 *)(p + (o)))
static inline s32 EnemyD459_CellIndex(struct PHVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline void EnemyD459_StepAnimation(u8 *p, void *sprite, struct FanSeq *s)
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
s32 EnemyD459_InitializeFromScript(u8 *p, void *parent)
{
    struct SpawnOffsets offsets = gUnk_0824FE5C;
    void *resource;
    register void *sprite asm("r9");
    register s16 *position asm("r8");
    s16 *zp;
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
        W(0x154) = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        H(0x134) = Script_GetValue();
    else
        H(0x134) = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *h = &B(0x132);
        *h = v;
        *h = (*h - 1) / 2;
        if ((u8)*h > 3)
            *h = 1;
    } else
        B(0x132) = 1;
    if (Script_SeekToKeyword('r'))
        W(0x14c) = Script_GetValueSafe();
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
        W(0x150) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x170) = Script_GetValue();
        W(0x174) = Script_GetValue();
        W(0x178) = Script_GetValue();
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
    *(void **)(p + 0x15c) = parent;
    {
        register u8 *graphics asm("r4") = p + 0x50;
        void *earlySprite;
        s16 *x, *z;
        register s8 *h asm("ip");
        AnimData_Load(graphics, 0x7e82);
        earlySprite = p + 0x24;
        Sprite_Init(earlySprite, graphics, 1);
        x = &H(0x40);
        *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
        h = &B(0x132);
        *x += offsets.e[*h].x << 2;
        z = &H(0x44);
        *z += offsets.e[*h].z << 2;
        sprite = earlySprite;
        position = x;
        zp = z;
        if (*h <= 1)
            *zp -= 0x50;
        else
            *position -= 0x50;
        AnimData_SelectPaletteBank(graphics, 0x227);
    }
    resource = ScriptProc_Start(0x922e, 0xfa45);
    *(void **)(p + 0x158) = resource;
    dead = H(0x144);
    if (dead == 0) {
        *(void **)(p + 0x108) = EnemyD459_UpdateAttackTiming;
        Anim_SetSequence(p + 0x10c, resource, 2, (u8)B(0x132), dead);
    } else {
        *(void **)(p + 0x108) = EnemyD459_UpdateDeath;
        H(0x12e) = 1;
        Anim_SetSequence(p + 0x10c, *(void **)(p + 0x158), 4, 0, 0);
        *position = U(0x1c) + 0x40;
        *zp = U(0x20) + 0x40;
        *position -= 0x60;
        *zp -= 0x60;
    }
    EnemyD459_StepAnimation(p, sprite, (struct FanSeq *)(p + 0x10c));
    if (H(0x144) == 0)
        EnemyD459_InitCollider(p);
    if (W(0x154) && H(0x144) == 0) {
        s32 cell = EnemyD459_CellIndex((struct PHVec *)position);
        TerrainModifiers_RegisterCell(p + 0x160, cell, 0, H(0x42) >> 8, 0, 1);
    }
    if (H(0x140)) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *(void **)(p + 0x15c), position, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (W(0x14c))
        EnemyD459_ReadScriptValueList(p);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0820539C);
#endif
