// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
s32 SceneMessages_RegisterReceiver(u8 *, s32, s32);
static inline u8 IsScenePaused(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
void SceneActorRegistry_RegisterCategoryReceiver(struct SceneActorRegistryActor *actor)
{
    u32 category = actor->category;
    u32 paused = IsScenePaused();
    if (!paused &&
        (category == 4 || category == 5 || category == 7 || category == 6 || category == 8 ||
         category == 21 || category == 25 || category == 23 || category == 18)) {
        SceneMessages_RegisterReceiver((u8 *)actor + 0xc, actor->id, 3);
        {
            u32 mask = 0x100;
            register u32 offset asm("r1") = 0x2c0;
            u16 *field;
            register u32 value asm("r1");
            field = (void *)((u8 *)actor + offset);
            value = *field;
            mask |= value;
            *field = mask;
        }
    }
}
