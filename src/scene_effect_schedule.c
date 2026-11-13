#include "scene_effect_schedule.h"
#include "engine.h"
#include "game_state.h"
#include "ram.h"

static inline u8 SceneEffectSchedule_TestOccupied(u32 mask, s32 bit, s32 index)
{
    if (mask & (bit << index))
        return TRUE;
    return FALSE;
}

struct SceneEffectScheduleEntry *SceneEffectSchedule_FindById(u32 id)
{
    struct SceneEffectSchedule *manager = gSceneEffectSchedule;
    s32 index;
    u32 mask;
    s32 bit;
    struct SceneEffectSchedule *cursor;
    if (!manager)
        return 0;
    index = 0;
    mask = manager->occupiedMask;
    cursor = manager;
    bit = 1;
    for (; index < 12; index++) {
        if (SceneEffectSchedule_TestOccupied(mask, bit, index)) {
            u32 offset = index * sizeof(manager->entries[0]);
            if (cursor->entries[0].id == id)
                return (void *)((u8 *)manager + (offset + sizeof(manager->task)));
        }
        cursor = (void *)((u8 *)cursor + sizeof(manager->entries[0]));
    }
    return 0;
}

static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}

struct SceneEffectScheduleEntry *SceneEffectSchedule_Allocate(struct SceneEffectSchedule *manager)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!Occupied(&manager->occupiedMask, i)) {
            manager->occupiedMask |= 1 << i;
            manager->entries[i].index = i;
            return &manager->entries[i];
        }
    }
    return 0;
}

s32 Div(s32, s32);

static inline s32 Scale(s32 input)
{
    s32 value = input;
    u32 flags = gEngineFlags;
    u32 mask = 0x800;
    if (flags & mask)
        return value;
    return Div((10 - gGameState->unk876) * value, 10);
}

s32 SceneEffectSchedule_Tick(struct SceneEffectSchedule *manager)
{
    struct SceneEffectScheduleEntry *initial = manager->entries;
    struct SceneEffectScheduleEntry *entry;
    s32 i;
    if (!manager->occupiedMask)
        return 0;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    entry = initial;
    for (i = 11; i >= 0; entry++, i--) {
        if (entry->effect && (s16)entry->effect->slot < 0) {
            s32 scale = Scale(entry->scaleValue);
            s32 random = Scale(entry->randomValue);
            entry->effect = SceneScheduledEffect_Create(
                &entry->position, entry->id, scale, entry->kind, entry->style, -1, entry->offset.x,
                entry->offset.y, entry->offset.z, random, entry->flags);
        }
    }
    return 0;
}

void SceneEffectSchedule_Destroy(void *unused)
{
    gSceneEffectSchedule = 0;
}

void SceneScheduledEffect_EnsureSingleton(void);

s32 SceneEffectSchedule_Initialize(struct SceneEffectSchedule *manager, u32 unusedArgument)
{
    SceneScheduledEffect_EnsureSingleton();
    gSceneEffectSchedule = manager;
    return 0;
}

struct SceneEffectSchedule *SceneEffectSchedule_Create(u32 argument)
{
    struct SceneEffectSchedule *manager;
    if (gSceneEffectSchedule)
        return gSceneEffectSchedule;
    manager = Task_Create(10, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))SceneEffectSchedule_Tick,
                          (void (*)(void *))SceneEffectSchedule_Destroy);
        if (SceneEffectSchedule_Initialize(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + RANDOM_TABLE_ADDR);
}

static inline s32 scaleByDifficulty(s32 value)
{
    return Div((10 - gGameState->unk876) * value, 10);
}

static inline s32 scaleUnlessFlagged(s32 value)
{
    if (gEngineFlags & 0x800)
        return value;
    return scaleByDifficulty(value);
}

/* Registry routine FD6B: schedules a scene effect (unless engine flag 0x400 is
   set): -p position, -a vector, -f kind then a random delay in [min, max), -n,
   -d, -t time, -s; time and delay shrink with game state +0x876 unless engine
   flag 0x800 is set (tentative). */

void SceneEffectSchedule_AddFromScript(void)
{
    struct SceneEffectScheduleEntry *entry;
    s32 time, delay;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return;
    }
    if (!gSceneEffectSchedule)
        return;
    entry = SceneEffectSchedule_Allocate(gSceneEffectSchedule);
    if (!entry)
        return;
    if (Script_SeekToKeyword('p')) {
        entry->position.x = Script_GetValue();
        entry->position.y = Script_GetValue();
        entry->position.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        entry->offset.x = Script_GetValue();
        entry->offset.y = Script_GetValue();
        entry->offset.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('f')) {
        s32 min, range;
        entry->style = Script_GetValue();
        min = Script_GetValue();
        range = Script_GetValue() - min;
        if (range == 0)
            entry->randomValue = range;
        else
            entry->randomValue = min + Mod(Random(), range);
    }
    entry->id = Script_GetKeywordValue('n', 0);
    entry->kind = Script_GetKeywordValue('d', 0);
    entry->scaleValue = Script_GetKeywordValue('t', 0);
    entry->flags = 0;
    if (Script_GetKeywordValue('s', 0))
        entry->flags = 1;
    time = scaleUnlessFlagged(entry->scaleValue);
    delay = scaleUnlessFlagged(entry->randomValue);
    entry->effect = SceneScheduledEffect_Create(&entry->position, entry->id, time, entry->kind,
                                                entry->style, -1, entry->offset.x, entry->offset.y,
                                                entry->offset.z, delay, entry->flags);
}

void SceneEffectSchedule_Reset(void)
{
    gSceneEffectSchedule = 0;
}

struct SceneEffectScheduleEntry *SceneEffectSchedule_FindById(u32);

void SceneEffectSchedule_RemoveFromScript(void)
{
    struct SceneEffectSchedule *manager = gSceneEffectSchedule;
    s32 id;
    struct SceneEffectScheduleEntry *entry;
    if (!manager)
        return;
    id = Script_GetKeywordValue('n', -1);
    if (id < 0)
        return;
    entry = SceneEffectSchedule_FindById(id);
    if (!entry)
        return;
    {
        s32 index = entry->index;
        u32 *mask;
        mask = &manager->occupiedMask;
        *mask &= ~(1u << index);
    }

    Memory_Clear(entry, sizeof(*entry));
}
