#ifndef GUARD_SCENE_ACTOR_POSITION_SEARCH_H
#define GUARD_SCENE_ACTOR_POSITION_SEARCH_H
#include "contact_actor.h"
/* Narrow source prefix shared by the neighboring grid-position search. */
struct SceneActorPositionSource {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
struct SceneActorTaggedPosition {
    u8 unknown0[0xC];
    struct ContactPosition position;
    u8 unknown14[4];
    u32 tag;
};
u32 sub_0822B694(u16 *coordinates, u32 index);
void sub_0807CDC4(const struct SceneActorPositionSource *, void *, struct ContactPosition *,
                  const struct ContactPosition *);
#endif
