#include "scene/actor/scene_actor_record.h"
struct SceneActorRecord *SceneActorRecord_FindPair(u32 key, u32 id)
{
    s32 i = 0;
    u8 *base = (u8 *)gGameState;
    struct SceneActorRecord *entry;
    struct SceneActorRecord *result;
    *(&entry) = (void *)(base + 0x670);
    result = entry;
    do {
        if ((entry->key == key) && (entry->id == id)) {
            return result;
        }
        entry++;
        result++;
        i++;
    } while (i <= 19);
    return 0;
}
