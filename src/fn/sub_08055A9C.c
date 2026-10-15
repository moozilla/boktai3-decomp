#include "script_keyframe_pool.h"
u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        struct ScriptKeyframeEntry *entry = manager->entries;
        do {
            ScriptKeyframe_UpdateEntry(entry);
            entry++;
            i++;
        } while (i < manager->count);
    }
    return 0;
}
