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
void sub_08076034(struct SceneActorAnimationView *, s32);
void sub_08076174(struct SceneActorAnimationView *, s32, u32);
void sub_080762A8(struct SceneActorAnimationView *, s32, u32);
void sub_080763D0(struct SceneActorAnimationView *, s32, u32);
void sub_080764C8(struct SceneActorAnimationView *, s32, u32);
void sub_080765F8(struct SceneActorAnimationView *, s32, u32, u32);
void sub_080766F8(struct SceneActorAnimationView *, s32);
void sub_08076818(struct SceneActorAnimationView *, s32);
#endif
