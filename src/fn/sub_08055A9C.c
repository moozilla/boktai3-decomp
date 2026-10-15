#include "script_keyframe_pool.h"
#define ScriptKeyframe_TickManager sub_08055A9C
u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        struct ScriptKeyframeEntry *entry = manager->entries;
        do {
            sub_08055828(entry);
            entry++;
            i++;
        } while (i < manager->count);
    }
    return 0;
}
