#include "monsters/cannons/wind_cannon.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
void *WindCannonManager_Create(void);
struct FanCannon *WindCannonManager_FindFreeSlot(struct FanCannonManager *, s32 *);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *Script_GetValueSafe(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void WindCannon_UpdateInactive(void);
void CombatActor_UpdateDefeatEffect(void);
void sub_082093F8(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, s32, void *, u32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void WindCannon_ReadToggleSchedule(struct FanCannon *);
static inline s32 Fan_Cell(struct ContactPosition *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
// confidence=low: script options initialize an eight-slot fan cannon and publish its occupied bit.
s32 WindCannon_CreateFromScript(void)
{
    struct FanCannonManager *manager = gFanCannonManager;
    struct FanCannon *p;
    s32 slot;
    void *resource;
    if (!manager) {
        WindCannonManager_Create();
        return -1;
    }
    p = WindCannonManager_FindFreeSlot(manager, &slot);
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
    AnimData_Load(p->graphics, 0x7e7d);
    Sprite_Init(&p->sprite.flags, p->graphics, 1);
    p->sprite.position = p->initial;
    if (!p->reverse) {
        p->palette = 0x1bf;
        p->variant = 0;
    } else {
        p->palette = 0x1c0;
        p->variant = 4;
    }
    AnimData_SelectPaletteBank(p->graphics, p->palette);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->resource = resource;
    if (!p->initialDead) {
        p->update = WindCannon_UpdateInactive;
        Anim_SetSequence(&p->sequence, resource, 0x15, (u8)p->direction, (u8)p->variant);
    } else {
        p->update = CombatActor_UpdateDefeatEffect;
        p->dead = 1;
        Anim_SetSequence(&p->sequence, p->resource, 0x16, (u8)p->direction, (u8)p->variant);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite.flags,
                           &p->sequence);
    if (!p->initialDead)
        sub_082093F8(p);
    if (p->terrain && !p->initialDead) {
        s32 cell = Fan_Cell(&p->sprite.position);
        TerrainModifiers_RegisterCell(p->terrainCell, cell, 0, p->sprite.position.y >> 8, 0, 0x803);
    }
    if (p->hasBody) {
        u32 heading = (u8)((p->direction << 6) + 0xc0);
        void *body = p->body;
        SceneBody_Init(body, p->name, &p->sprite.position, heading, 4, p);
        SceneBody_SetSprite(body, &p->sprite.flags);
        TerrainOverlay_QueueAddActor(body);
    }
    if (p->schedule)
        WindCannon_ReadToggleSchedule(p);
    manager->occupiedSlots |= 1 << slot;
    return slot;
}
