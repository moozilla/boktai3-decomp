// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_CreateEntryFromScript sub_0803D61C
struct ScenePoolManager *sub_0803BA84(void);
struct ScenePoolEntry *sub_0803D5E8(struct ScenePoolManager *, u32 *);
void sub_0803CDF0(struct ScenePoolEntry *);
s32 sub_0803C914(struct ScenePoolManager *, struct ScenePoolEntry *);
s32 ScenePool_CreateEntryFromScript(void)
{
    struct ScenePoolManager *manager = sub_0803BA84();
    struct ScenePoolEntry *entry;
    u32 index;
    s32 result;
    if (!manager)
        goto failure;
    asm("" : "+r"(manager));
    entry = sub_0803D5E8(manager, &index);
    if (entry)
        goto create;
failure:
    return -1;
create:
    sub_0803CDF0(entry);
    result = sub_0803C914(manager, entry);
    entry->step = 0x200;
    entry->acceleration = 0x4000;
    entry->speed = 0x100;
    entry->turnStep = 0x200;
    entry->motionPhase = 0;
    entry->index = index;
    entry->active = 1;
    manager->activeCount++;
    return result;
}
