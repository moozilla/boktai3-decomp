#ifndef GUARD_SCENE_ACTOR_STATE_H
#define GUARD_SCENE_ACTOR_STATE_H

#include "contact_collider.h"
#include "scene_actor_components.h"
#include "scene_actor_group.h"
#include "room_actor.h"

/* Extended reset-phase view; fields are offset names unless consumers establish a role. */

/* Motion/reaction block embedded at actor offset 0xDC. */
struct SceneActorReactionMotion {
    u32 fieldDC;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};
struct SceneActorRegistryResetView {
    struct RoomActor base;
    struct SceneActorChaseGroup group;
    u8 animation624[0x38];
    u16 animationFlags65C;
    u8 unknown65E[0x1A];
    u32 animationFlags678;
    s16 field67C;
    u8 unknown67E[4];
    s16 field682;
    u8 unknown684[4];
    void *field688;
    u8 unknown68C;
    u8 timer68D;
    u8 direction68E, unknown68F, savedDirection690;
    u8 unknown691[7];
    u16 field698;
    u16 frame69A;
    u16 animationFrame69C;
    u16 field69E;
    u8 unknown6A0[7];
    u8 animationFlag6A7;
    u8 unknown6A8[4];
    u16 field6AC;
    u8 unknown6AE[0x22];
    u16 field6D0;
    u8 unknown6D2[2];
    u16 flags6D4;
    u16 field6D6;
    struct ContactPosition vector6D8;
    struct ContactPosition vector6E0;
    u16 field6E8;
};

#endif
