#include "script_keyframe_pool.h"
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
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
