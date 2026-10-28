// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct FanVec {
    s16 x, y, z, pad;
};
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct FanCannon {
    s32 age;
    s32 name;
    struct FanVec initial;
    u32 flags;
    u8 p14[0xc];
    u16 frame;
    u8 p22[0xa];
    struct FanVec position;
    u8 p34[8];
    u8 graphics[0x1c];
    u8 collider[0x54];
    u8 body[0x48];
    void (*update)(void);
    struct FanSeq sequence;
    s32 range;
    s16 maximumWind, health, damage, reaction;
    s16 f114, dead, flash;
    s8 direction;
    u8 reverse;
    s16 period, palette, wind, variant;
    u8 activated, stopped;
    s16 hasBody;
    s16 maximumHealth, hudA, hudB, f12e, f130, f132, hudTime, initialDead;
    void *schedule;
    s32 f13c, terrain;
    void *resource;
    u8 terrainCell[8];
    s32 f150, f154, f158, f15c, f160, f164, f168;
    s16 scheduleCount, scheduleIndex;
    s16 times[8];
};
struct FanManager {
    u8 p0[0x18];
    u32 used;
};
extern u32 gTerrainWidth, gTerrainHeight;
struct FanTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
extern struct FanTerrain *gTerrainMap;
void *FanCannonManager_Create(void);
struct FanCannon *FanCannonManager_FindFreeSlot(struct FanManager *, s32 *);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *Script_GetValueSafe(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u32);
void FanCannon_UpdateInactive(void);
void CombatActor_UpdateDefeatEffect(void);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void sub_082093F8(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, s32, void *, u32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void FanCannon_ReadToggleSchedule(struct FanCannon *);
static inline s32 Fan_Cell(struct FanVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline void Fan_Step(struct FanCannon *p, void *sprite, struct FanSeq *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r7");
    register u32 savedDuration asm("r3");
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
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*c & 0x30) >> 4) & bit))
            p->flags |= 4;
        else
            p->flags &= ~4;
    }
    {
        u32 input = s->flags, bit = 2;
        register u32 first asm("r1");
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
            register s32 w asm("r1") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
// confidence=low: script options initialize an eight-slot fan cannon and publish its occupied bit.
s32 FanCannon_CreateFromScript(void)
{
    struct FanManager *manager = gFanCannonManager;
    struct FanCannon *p;
    s32 slot;
    void *graphics;
    register void *resource asm("r7");
    register void *earlySprite asm("r5");
    register void *sprite asm("r8");
    register struct FanSeq *sequence asm("r4");
    if (!manager) {
        FanCannonManager_Create();
        return -1;
    }
    p = FanCannonManager_FindFreeSlot(manager, &slot);
    if (!p)
        return -1;
    if (Script_SeekToKeyword('p')) {
        p->initial.x = Script_GetValue();
        p->initial.y = Script_GetValue();
        p->initial.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a'))
        p->maximumWind = Script_GetValue();
    else
        p->maximumWind = 15;
    if (Script_SeekToKeyword('l')) {
        s32 v = Script_GetValue();
        p->maximumHealth = v;
        p->health = v;
        if ((s16)v < 0)
            p->initialDead = 1;
    } else {
        p->maximumHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->terrain = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->period = Script_GetValue();
    else
        p->period = 300;
    if (Script_SeekToKeyword('h')) {
        p->direction = Script_GetValue();
        p->direction = (p->direction - 1) / 2;
        if ((u8)p->direction > 3)
            p->direction = 1;
        if (p->direction <= 1)
            p->direction = 0;
        else
            p->direction = 1;
    } else
        p->direction = 0;
    if (Script_SeekToKeyword('r'))
        p->schedule = Script_GetValueSafe();
    if (Script_SeekToKeyword('t'))
        p->reverse = Script_GetValue();
    if (Script_SeekToKeyword('R'))
        p->range = Script_GetValue();
    else
        p->range = 0x600;
    if (Script_SeekToKeyword('n'))
        p->name = Script_GetValue();
    if (Script_SeekToKeyword('S'))
        p->hasBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->f12e = Script_GetValue();
        p->hudB = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudA = Script_GetValue();
    else
        p->hudA = 2;
    if (Script_SeekToKeyword('D'))
        p->f13c = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->f160 = Script_GetValue();
        p->f164 = Script_GetValue();
        p->f168 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->f130 = Script_GetValue();
        p->f132 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudTime = Script_GetValue();
        if (p->hudTime > 99)
            p->hudTime = 99;
        if (p->hudTime <= 0)
            p->hudTime = 1;
    } else
        p->hudTime = 1;
    graphics = p->graphics;
    AnimData_Load(graphics, 0x7e7d);
    earlySprite = &p->flags;
    Sprite_Init(earlySprite, graphics, 1);
    p->position = p->initial;
    {
        u32 reverse = p->reverse;
        sprite = earlySprite;
        if (!reverse) {
            p->palette = 0x1bf;
            p->variant = 0;
        } else {
            p->palette = 0x1c0;
            p->variant = 4;
        }
    }
    AnimData_SelectPaletteBank(graphics, p->palette);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->resource = resource;
    if (!p->initialDead) {
        p->update = FanCannon_UpdateInactive;
        sequence = &p->sequence;
        Anim_SetSequence(sequence, resource, 0x15, (u8)p->direction, (u8)p->variant);
    } else {
        p->update = CombatActor_UpdateDefeatEffect;
        p->dead = 1;
        sequence = &p->sequence;
        Anim_SetSequence(sequence, p->resource, 0x16, (u8)p->direction, (u8)p->variant);
    }
    Fan_Step(p, sprite, sequence);
    if (!p->initialDead)
        sub_082093F8(p);
    if (p->terrain && !p->initialDead) {
        s32 cell = Fan_Cell(&p->position);
        TerrainModifiers_RegisterCell(p->terrainCell, cell, 0, p->position.y >> 8, 0, 0x803);
    }
    if (p->hasBody) {
        u32 heading = (u8)((p->direction << 6) + 0xc0);
        void *body = p->body;
        SceneBody_Init(body, p->name, &p->position, heading, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (p->schedule)
        FanCannon_ReadToggleSchedule(p);
    manager->used |= 1 << slot;
    return slot;
}
