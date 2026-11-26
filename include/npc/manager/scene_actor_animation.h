#ifndef GUARD_SCENE_ACTOR_ANIMATION_H
#define GUARD_SCENE_ACTOR_ANIMATION_H
#include "scene/actor/scene_actor_profile.h"
#include "link/session/room_actor.h"
#include "engine/video/display_sequence.h"
#include "ui/widgets/scene_input_widget.h"
#include "engine/video/sprite.h"
/* Animation-phase actor and drawable views; selection names are provisional. */
struct SceneActorAnimationLink {
    u8 unknown0[0x6CC];
    u32 phaseTimer;
    u8 unknown6D0[4];
    u16 flags;
    u8 unknown6D6[0x12];
    u16 phaseDuration;
};
struct SceneActorAnimationSequenceDrawable {
    struct DisplaySequenceSprite sprite;
    struct SpriteRenderResource graphics;
    struct DisplaySequenceState sequence;
    const u8 *resource;
};
struct SceneActorAnimationTileDrawable {
    struct SceneInputResource resource;
    struct SceneInputWidget tile;
};
void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);
void SceneActorAnimation_SelectDirectionalEntryFlags(struct RoomActor *, s32, u32);
void sub_080762A8(struct RoomActor *, s32, u32);
void SceneActorAnimation_SelectEntryFlags(struct RoomActor *, s32, u32);
void SceneActorAnimation_SelectDirectionFlagsVariant(struct RoomActor *, s32, u32);
void SceneActorAnimation_SelectExplicitVariant(struct RoomActor *, s32, u32, u32);
void SceneActorAnimation_SelectDirectionalFlags(struct RoomActor *, s32);
void SceneActorAnimation_SelectProfileEntry(struct RoomActor *, s32);
#endif
