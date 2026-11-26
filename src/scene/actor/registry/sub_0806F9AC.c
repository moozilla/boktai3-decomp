#include "scene/actor/scene_actor_record.h"
struct SceneActorRecord *SceneActorRecord_FindEmpty(void)
{
    s32 i = 0;
    struct SceneActorRecord *table = SCENE_ACTOR_RECORDS;
    for (; i < 20; i++) {
        if (!table[i].id)
            return &table[i];
    }
    return NULL;
}
