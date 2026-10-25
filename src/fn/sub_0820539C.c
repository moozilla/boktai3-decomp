// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.5%
// fix.py best (sweep after the inlined-insn fix)
// Register choice for the z pointer (r7 in the ROM), the facing-index pointer
// reloads and the Anim_SetSequence argument setup still differ.
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
        frame = *c >> 6;
        *(u16 *)((u8 *)sprite + 0x10) = frame;
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
static inline void EnemyD459_StepAnimation_InitializeFromScript(u8 *p, void *sprite,
                                                                struct FanSeq *s)
{
    u32 flags;
    u32 elapsed;
    u32 duration;
    register u32 mask asm("r7");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    {
        u32 frame;
        frame = *c >> 6;
        *(u16 *)((u8 *)sprite + 0x10) = frame;
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

inline static u32 Arg1(u32 k_, u8 *p)
{
    return p + k_;
}

s32 EnemyD459_InitializeFromScript(u8 *p, void *parent)
{
    struct SpawnOffsets offsets = gUnk_0824FE5C;
    void *resource;
    void **slot;
    register void *sprite asm("r9");
    s16 *position;
    s16 *zp;
    s32 dead;
    if (Script_SeekToKeyword('p')) {
        *((s16 *)(p + 0x1c)) = Script_GetValue();
        *((s16 *)(p + 0x1e)) = Script_GetValue();
        *((s16 *)(p + 0x20)) = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        *((s16 *)(p + 0x120)) = Script_GetValue();
        *((s16 *)(p + 0x122)) = Script_GetValue();
        *((s16 *)(p + 0x124)) = Script_GetValue();
        *((s16 *)(p + 0x136)) = Script_GetValue();
    } else {
        *((s16 *)(p + 0x120)) = 30;
        *((s16 *)(p + 0x122)) = 30;
        *((s16 *)(p + 0x124)) = 30;
        *((s16 *)(p + 0x136)) = 24;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        *((s16 *)(p + 0x138)) = health;
        *((s16 *)(p + 0x126)) = health;
        if (((s16)health) < 0)
            *((s16 *)(p + 0x144)) = 1;
    } else {
        *((s16 *)(p + 0x138)) = 120;
        *((s16 *)(p + 0x126)) = 120;
    }
    if (Script_SeekToKeyword('d'))
        *((u32 *)(p + 0x154)) = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        *((s16 *)(p + 0x134)) = Script_GetValue();
    else
        *((s16 *)(p + 0x134)) = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *h = &(*((s8 *)(p + 0x132)));
        *h = v;
        *h = ((*h) - 1) / 2;
        if (((u8)(*h)) > 3)
            *h = 1;
    } else
        *((s8 *)(p + 0x132)) = 1;
    if (Script_SeekToKeyword('r'))
        *((u32 *)(p + 0x14c)) = Script_GetValueSafe();
    if (Script_SeekToKeyword('S'))
        *((s16 *)(p + 0x140)) = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        *((s16 *)(p + 0x13a)) = Script_GetValue();
        *((s16 *)(p + 0x13c)) = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        *((s16 *)(p + 0x13e)) = Script_GetValue();
    else
        *((s16 *)(p + 0x13e)) = 2;
    if (Script_SeekToKeyword('D'))
        *((u32 *)(p + 0x150)) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        *((u32 *)(p + 0x170)) = Script_GetValue();
        *((u32 *)(p + 0x174)) = Script_GetValue();
        *((u32 *)(p + 0x178)) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        *((s16 *)(p + 0x148)) = Script_GetValue();
        *((s16 *)(p + 0x14a)) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        *((s16 *)(p + 0x142)) = Script_GetValue();
        if ((*((s16 *)(p + 0x142))) > 99)
            *((s16 *)(p + 0x142)) = 99;
        if ((*((s16 *)(p + 0x142))) <= 0)
            *((s16 *)(p + 0x142)) = 1;
    } else
        *((s16 *)(p + 0x142)) = 0;
    *((void **)(p + 0x15c)) = parent;
    {
        register u8 *graphics asm("r4") = p + 0x50;
        void *earlySprite;
        s16 *x;
        s16 *z;
        register s8 *h asm("ip");
        AnimData_Load(graphics, 0x7e82);
        earlySprite = p + 0x24;
        Sprite_Init(earlySprite, graphics, 1);
        x = &(*((s16 *)(p + 0x40)));
        *((struct SceneVec *)(p + 0x40)) = *((struct SceneVec *)(p + 0x1c));
        h = &(*((s8 *)(p + 0x132)));
        *x += offsets.e[*h].x << 2;
        z = &(*((s16 *)(p + 0x44)));
        *z += offsets.e[*h].z << 2;
        {
            s32 facing = *h;
            sprite = earlySprite;
            position = x;
            zp = z;
            if (facing <= 1)
                *zp -= 0x50;
            else
                *position -= 0x50;
            asm("" : : "r"(earlySprite));
        }
        AnimData_SelectPaletteBank(graphics, 0x227);
    }
    resource = ScriptProc_Start(0x922e, 0xfa45);
    slot = (void **)(p + 0x158);
    *slot = resource;
    dead = *((s16 *)(p + 0x144));
    if (dead == 0) {
        *((void **)(p + 0x108)) = EnemyD459_UpdateAttackTiming;
        Anim_SetSequence(p + 0x10c, resource, 2, (u8)(*((s8 *)(p + 0x132))), dead);
    } else {
        *((void **)(p + 0x108)) = EnemyD459_UpdateDeath;
        {
            s16 *d = &(*((s16 *)(p + 0x12e)));
            s32 zero = 0;
            *d = 1;
            Anim_SetSequence(p + 0x10c, *slot, 4, 0, zero);
        }
        *position = (*((u16 *)(p + 0x1c))) + 0x40;
        *zp = (*((u16 *)(p + 0x20))) + 0x40;
        *position -= 0x60;
        *zp -= 0x60;
    }
    EnemyD459_StepAnimation_InitializeFromScript(p, sprite, (struct FanSeq *)(p + 0x10c));
    if ((*((s16 *)Arg1(0x144, p))) == 0)
        EnemyD459_InitCollider(p);
    if ((*((u32 *)(p + 0x154))) && ((*((s16 *)(p + 0x144))) == 0)) {
        s32 cell = EnemyD459_CellIndex((struct PHVec *)position);
        TerrainModifiers_RegisterCell(p + 0x160, cell, 0, (*((s16 *)(p + 0x42))) >> 8, 0, 1);
    }
    if (*((s16 *)(p + 0x140))) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *((void **)(p + 0x15c)), position, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (*((u32 *)(p + 0x14c)))
        EnemyD459_ReadScriptValueList(p);
}

#else
INCLUDE_ASM("asm/nonmatching", EnemyD459_InitializeFromScript);
#endif
