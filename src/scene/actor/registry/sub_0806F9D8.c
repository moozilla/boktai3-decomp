#include "scene/actor/scene_actor_record.h"
struct SceneActorRecord *SceneActorRecord_FindPair(u32 key, u32 id)
{
    s32 i = 0;
    struct SceneActorRecord *table = SCENE_ACTOR_RECORDS;
    for (; i < 20; i++) {
        if (table[i].key == key && table[i].id == id)
            return &table[i];
    }
    return NULL;
}
