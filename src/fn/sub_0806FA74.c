#include "scene_actor_record.h"
void SceneActorRecord_RecordPair(u32 keyWord, u32 idWord)
{
    u16 key = keyWord, id = idWord;
    struct SceneActorRecord *entry;
    struct SceneActorRegistryManager *manager;
    entry = SceneActorRecord_FindPair(key, id);
    if (entry)
        return;
    entry = SceneActorRecord_FindEmpty();
    if (!entry)
        return;
    manager = SceneActorRegistry_GetManager();
    if (!manager)
        return;
    entry->key = key;
    entry->id = id;
    entry->ticks = manager->mode;
}
