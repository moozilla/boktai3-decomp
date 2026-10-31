#include "scene_pool.h"
struct ScenePoolManager *ScenePool_GetInstance(void);
struct ScenePoolEntry *ScenePool_FindFreeEntry(struct ScenePoolManager *, u32 *);
void ScenePool_ParseEntryParameters(struct ScenePoolEntry *);
void ScenePool_RegisterEntry(struct ScenePoolManager *, struct ScenePoolEntry *);
s32 ScenePool_CreateEntryFromScript(void)
{
    struct ScenePoolManager *manager = ScenePool_GetInstance();
    struct ScenePoolEntry *entry;
    u32 index;
    if (!manager)
        return -1;
    entry = ScenePool_FindFreeEntry(manager, &index);
    if (!entry)
        return -1;
    ScenePool_ParseEntryParameters(entry);
    ScenePool_RegisterEntry(manager, entry);
    entry->step = 0x200;
    entry->acceleration = 0x4000;
    entry->speed = 0x100;
    entry->turnStep = 0x200;
    entry->motionPhase = 0;
    entry->index = index;
    entry->active = 1;
    manager->activeCount++;
    /* Success leaves the original unused r0 value; only failure returns -1. */
}
