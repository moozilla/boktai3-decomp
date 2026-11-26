#ifndef GUARD_LUN_H
#define GUARD_LUN_H

#include "link/session/room_actor.h"
#include "scene/actor/scene_actor_group.h"
#include "scene/motion/scene_actor_motion.h"
#include "scene/motion/scene_actor_route.h"

/* Lun's class fields past the room actor (Lun_Create allocates 0x44C bytes; provisional). */
struct Lun {
    struct RoomActor base;
    u32 flags3D4;
    u8 parameter3D8;
    u8 parameter3D9;
    u16 participantProgress;
    u16 ground3DC;
    u8 unknown3DE[2];
    u16 curveTime;
    u8 hoverPhase;
    s8 swayStep;
    s16 sway;
    u8 participantElapsed, participantStart;
    struct SceneActorMotionSubject curveSubject;
    u8 unknown3FC[0x18];
    struct ContactPosition curveStart, curveEnd, routeMotion;
    /* Script placement and route playback share this cursor. */
    union {
        struct SceneActorRegistryRoute registry;
        struct SceneActorRouteCursor cursor;
    } route;
    u8 unknown440[2];
    u16 field442, routeDelay;
    u8 unknown446[6];
};

#endif // GUARD_LUN_H
