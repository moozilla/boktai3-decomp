#ifndef GUARD_BAT_H
#define GUARD_BAT_H

#include "room_actor.h"

/* Bat's class fields past the room actor (Bat_Create allocates 0x43C bytes; provisional). */
struct Bat {
    struct RoomActor base;
    u32 flags3D4;
    s16 height;
    u8 parameter3DA;
    u8 unknown3DB;
    u8 parameter3DC;
    u8 unknown3DD;
    s16 field3DE;
    u8 unknown3E0[0xA];
    s16 field3EA;
    u8 unknown3EC[0x48];
    struct ContactPosition initialPosition;
};

#endif // GUARD_BAT_H
