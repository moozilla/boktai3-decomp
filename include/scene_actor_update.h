#ifndef GUARD_SCENE_ACTOR_UPDATE_H
#define GUARD_SCENE_ACTOR_UPDATE_H
#include "scene_actor_behavior.h"
#include "scene_actor_contact_list.h"
/* Narrow update-phase views; offsets and names remain provisional. */
struct SceneActorUpdateControl {
    u8 unknown0[0x1F6];
    u8 timer;
};
struct SceneActorUpdateContact {
    u8 unknown0[0x45C];
    struct SceneActorContactNode *contact;
    u8 unknown460[0x1B8];
    void *cleanup;
};
#endif
