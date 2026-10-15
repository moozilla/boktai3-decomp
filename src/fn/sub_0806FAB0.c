#include "scene_actor_record.h"
u32 SceneActorRecord_CanRecordPair(u16 key, u16 id)
{
    if (!SceneActorRecord_FindPair(key, id) && SceneActorRecord_FindEmpty())
        return 1;
    return 0;
}
