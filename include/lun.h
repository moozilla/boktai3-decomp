#ifndef GUARD_LUN_H
#define GUARD_LUN_H

#include "room_actor.h"
#include "scene_actor_group.h"

/* Lun's class fields past the room actor (Lun_Create allocates 0x44C bytes; provisional). */
struct Lun {
    struct RoomActor base;
    u32 flags3D4;
    u8 parameter3D8;
    u8 parameter3D9;
    u8 unknown3DA[2];
    u16 ground3DC;
    u8 unknown3DE[0x4E];
    struct SceneActorRegistryRoute route;
    u8 unknown440[2];
    u16 field442;
    u8 unknown444[8];
};

#endif // GUARD_LUN_H
