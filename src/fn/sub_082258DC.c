#include "global.h"
#include "scene_body_registry.h"
struct Actor {
    u32 id;
    u8 pad[64];
    struct Actor *next;
};
struct Actor *SceneBody_FindById(u16 id)
{
    struct Actor *actor;
    if (!gSceneBodyRegistry)
        return 0;
    actor = gSceneBodyRegistry->first;
    while (actor) {
        if (actor->id == id)
            return actor;
        actor = actor->next;
    }
    return 0;
}
