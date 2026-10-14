#include "scene_pool.h"
#define ScenePool_ParseEntryParameters sub_0803CDF0

struct Values {
    s32 x, y, z, unused;
};
s32 ScenePool_ParseEntryParameters(struct ScenePoolEntry *entry)
{
    struct Values position;
    s32 range;
    entry->scriptId = Script_GetKeywordValue('n', 0);
    entry->kind = Script_GetKeywordValue('t', 1);
    entry->side = Script_GetKeywordValue('a', 0);
    entry->mode = Script_GetKeywordValue('l', 0);
    if (Script_SeekToKeyword('s')) {
        entry->duration = Script_GetValue();
        entry->parameter = Script_GetValue();
    } else {
        entry->duration = 500;
        entry->parameter = 20;
    }
    if (Script_SeekToKeyword('p')) {
        {
            struct Values *dst = &position;
            dst->x = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct Values *dst = &position;
            dst->y = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct Values *dst = &position;
            dst->z = Script_GetPc() ? Script_GetValue() : 0;
        }
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    {
        s32 *dst = entry->position;
        entry->position[0] = position.x << 12;
        dst[1] = position.y << 12;
        dst[2] = position.z << 12;
    }
    range = Script_GetKeywordValue('r', 20000);
    entry->origin = entry->position[0];
    entry->limit = entry->position[0] + (range << 12);
    entry->state = 0;
    entry->stateFrame = 0;
    entry->stateEntered = 1;
    Memory_Clear(entry->history, 0x100);
}
