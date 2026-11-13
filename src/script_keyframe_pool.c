#include "script_keyframe_pool.h"

struct ScriptKeyframeEntry *ScriptKeyframe_FindFromScript(void)
{
    struct ScriptKeyframeManager **manager = &gScriptKeyframeManager;
    struct ScriptKeyframeManager *current, *loaded;
    s32 loadedCount;
    u32 tag;
    u32 entryOffset;
    s32 index;
    s32 count;
    u16 *tagCursor;
    if (!*manager)
        goto missing;
    if (Script_SeekToKeyword('n'))
        goto search;
    goto missing;
found: {
    return (void *)((u8 *)current->entries + entryOffset);
}
search:
    tag = Script_GetValue();
    index = 0;
    loaded = *manager;
    loadedCount = loaded->count;
    if (index >= loadedCount)
        goto missing;
    current = loaded;
    count = loadedCount;
    tagCursor = &current->entries[0].tag;
    entryOffset = 0;
    do {
        if (*tagCursor == tag)
            goto found;
        tagCursor = (void *)((u8 *)tagCursor + sizeof(struct ScriptKeyframeEntry));
        entryOffset += sizeof(struct ScriptKeyframeEntry);
    } while (++index < count);
missing:
    return 0;
}

void ScriptKeyframe_SetFrameFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('p')) {
        union ScriptKeyframeAnimation *animation = &entry->animation;
        struct ProjectedDisplayResource *resource = &entry->resource;
        Anim_LoadKeyframe(animation, resource, (u16)Script_GetValue(), 0);
    }
}

void ScriptKeyframe_SetMirrorFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('f')) {
        if (Script_GetValue())
            entry->animation.display.flags |= 4;
        else
            entry->animation.display.flags &= ~4;
    }
}

void ScriptKeyframe_HideFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        u32 flags = entry->animation.display.flags;
        u32 mask = 1;
        flags |= mask;
        entry->animation.display.flags = flags;
        if (entry->conditionalVisibility)
            entry->hidden = mask;
    }
}

void ScriptKeyframe_ShowFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        entry->animation.display.flags &= ~1;
        if (entry->conditionalVisibility)
            entry->hidden = 0;
    }
}

void ScriptKeyframe_SetPriorityFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry && Script_SeekToKeyword('p'))
        entry->animation.keyframe.priority = Script_GetValue();
}

void ScriptKeyframe_SetTargetFromScript(void)
{
    struct ScriptKeyframeEntry *entry = ScriptKeyframe_FindFromScript();
    if (entry) {
        if (Script_SeekToKeyword('p')) {
            entry->target.x = Script_GetValue();
            entry->target.y = Script_GetValue();
            entry->target.z = Script_GetValue();
        }
        if (Script_SeekToKeyword('f'))
            entry->moveDuration = Script_GetValue();
        entry->origin = entry->position;
        entry->moveFrame = 0;
        entry->moving = 1;
    }
}

void ScriptKeyframe_UpdateEntry(struct ScriptKeyframeEntry *entry)
{
    if (entry->moving) {
        if (++entry->moveFrame >= entry->moveDuration) {
            entry->position = entry->target;
            entry->moving = 0;
        } else {
            s32 left = entry->moveDuration - entry->moveFrame;
            entry->position.x = Div(entry->target.x * entry->moveFrame + entry->origin.x * left,
                                    entry->moveDuration);
            entry->position.y = Div(entry->target.y * entry->moveFrame + entry->origin.y * left,
                                    entry->moveDuration);
            entry->position.z = Div(entry->target.z * entry->moveFrame + entry->origin.z * left,
                                    entry->moveDuration);
        }
        entry->animation.display.position = entry->position;
    }
    if (entry->conditionalVisibility) {
        if (!entry->hidden && gGameState->unk14)
            entry->animation.display.flags &= ~1;
        else
            entry->animation.display.flags |= 1;
    }
}

void ScriptKeyframe_AddFromScript(void)
{
    struct ScriptKeyframeManager *manager = gScriptKeyframeManager;
    struct ScriptKeyframeEntry *entry;
    struct ProjectedDisplayResource *resource;
    struct ContactPosition *position;
    u32 value, frame, mode, priority;
    if (!manager || manager->count >= manager->capacity)
        return;
    entry = &manager->entries[manager->count];
    value = Script_SeekToKeyword('n');
    if (value)
        entry->tag = Script_GetValue();
    else
        entry->tag = value;
    if (!Script_SeekToKeyword('d'))
        return;
    {
        u32 id = Script_GetValue();
        u32 key = 0xCB05;
        resource = ScriptProc_Start(key, id);
    }
    entry->resource = *resource;
    ResourceHeader_Relocate(&entry->resource, resource);
    if (Script_SeekToKeyword('i'))
        frame = Script_GetValue();
    else
        frame = 0;
    {
        u32 mirror;
        if (Script_SeekToKeyword('m'))
            mirror = Script_GetValue();
        else
            mirror = 0;
        mode = 0;
        if (mirror == 1)
            mode = 16;
    }
    if (Script_SeekToKeyword('R'))
        priority = Script_GetValue();
    else
        priority = 2;
    {
        u32 present = Script_SeekToKeyword('p');
        if (present) {
            u32 x = Script_GetValue();
            position = &entry->position;
            position->x = x;
            entry->position.y = Script_GetValue();
            entry->position.z = Script_GetValue();
        } else {
            entry->position.x = present;
            entry->position.y = present;
            entry->position.z = present;
            position = &entry->position;
        }
    }
    Widget_InitLinked(&entry->animation, &entry->resource, (u16)frame, mode, (u8)priority, 0, 60,
                      position);
    value = Script_SeekToKeyword('A');
    if (value)
        entry->conditionalVisibility = Script_GetValue();
    else
        entry->conditionalVisibility = value;
    entry->hidden = 0;
    gScriptKeyframeManager->count++;
}

u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *manager)
{
    s32 index = 0;
    if (index < manager->count) {
        struct ScriptKeyframeEntry *entry = manager->entries;
        do {
            ScriptKeyframe_UpdateEntry(entry);
            entry++;
            index++;
        } while (index < manager->count);
    }
    return 0;
}

s32 ScriptKeyframe_DestroyManager(struct ScriptKeyframeManager *manager)
{
    s32 index = 0;
    if (index < manager->count) {
        union ScriptKeyframeAnimation *animation = &manager->entries[0].animation;
        do {
            Anim_Remove(animation);
            animation = (void *)((u8 *)animation + sizeof(struct ScriptKeyframeEntry));
            index++;
        } while (index < manager->count);
    }
    gScriptKeyframeManager = 0;
    return 0;
}

s32 ScriptKeyframe_InitManager(struct ScriptKeyframeManager *manager, u32 unused1, u32 unused2,
                               u32 capacity)
{
    u32 zero = 0;
    manager->capacity = capacity;
    manager->count = zero;
    gScriptKeyframeManager = manager;
    return 0;
}

struct ScriptKeyframeManager *ScriptKeyframe_CreateManagerFromScript(u32 arg1, u32 arg2)
{
    if (gScriptKeyframeManager)
        return gScriptKeyframeManager;
    {
        struct ScriptKeyframeManager *manager;
        u32 capacity;
        if (Script_SeekToKeyword('m'))
            capacity = Script_GetValue();
        else
            capacity = 1;
        manager = Task_Create(8, capacity * sizeof(struct ScriptKeyframeEntry) + 0x1C);
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))ScriptKeyframe_TickManager,
                              (void (*)(void *))ScriptKeyframe_DestroyManager);
            if (ScriptKeyframe_InitManager(manager, arg1, arg2, capacity) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
        return manager;
    }
}
