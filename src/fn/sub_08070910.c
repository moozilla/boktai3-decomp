#include "global.h"

#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);

#ifdef NONMATCHING
// fuzzy: 36.1%
// scope shape 67 (differences once low registers are set aside)
// confidence=low: walks the scene actor list and, for available actors with flag 0x100 in flags2C0, resets their script state.
void sub_08070910(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    s32 keyword;

    if (!SceneActorRegistry_GetManager())
        return;
    node = gSceneActorRegistryNode;
    actor = node->actor;
    keyword = Script_GetKeywordValue(0x6d, 0);
    if (!actor)
        return;
    do {
        u32 unavailable;
        if (actor->flags2D4 & 0x40)
            unavailable = 1;
        else if (actor->flags2D0 & 0x20000)
            unavailable = 1;
        else if (actor->health <= 0)
            unavailable = 1;
        else
            unavailable = 0;
        if (!unavailable) {
            if (actor->flags2C0 & 0x100) {
                actor->state2A8 = 2;
                actor->phase2A9 = 0;
                actor->behavior2AA = 0;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
                if (keyword == 0) {
                    actor->field2B2 = 1;
                    actor->field2B0 = keyword;
                    actor->unknown2B5[0] = 16;
                    actor->control = SceneActorControl_OnIdle;
                    actor->field2C8 = keyword;
                    actor->flags114 &= ~1;
                    actor->field104 = keyword;
                }
                actor->flags2D4 |= 0x4000;
                actor->flags2D0 |= 8;
            }
        }
        node = node->next;
        actor = node->actor;
    } while (actor);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08070910);
#endif
