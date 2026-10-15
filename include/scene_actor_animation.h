#ifndef GUARD_SCENE_ACTOR_ANIMATION_H
#define GUARD_SCENE_ACTOR_ANIMATION_H
#include "scene_actor_profile.h"
/* Animation-phase actor and drawable views; selection names are provisional. */
struct SceneActorAnimationView {
    void *object;
    void *drawable;
    u8 drawableKind, unknown9[0x50];
    u8 direction;
    u8 unknown5A[0x259];
    u8 restart;
    u8 unknown2B4[3];
    u8 selection;
    u8 unknown2B8[0xC];
    u32 profileMode;
    u8 unknown2C8[0x40];
    u32 profileId;
};
struct SceneActorAnimationSequenceDrawable {
    u8 unknown0[0x48];
    u8 sequence[0x10];
    const void *resource;
};
struct SceneActorAnimationTile {
    u8 unknown0[8];
    u32 flags;
};
struct SceneActorAnimationTileDrawable {
    u8 unknown0[0x20];
    struct SceneActorAnimationTile tile;
};
u32 sub_08220C8C(void *, const void *, u16, u8, u8);
void sub_08220D78(struct SceneActorAnimationTile *, const void *, u16, u16, u16);
#endif
