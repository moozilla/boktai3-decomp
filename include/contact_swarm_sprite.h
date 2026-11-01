#ifndef GUARD_CONTACT_SWARM_SPRITE_H
#define GUARD_CONTACT_SWARM_SPRITE_H

#include "contact_position.h"

struct ContactSwarmSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
};

#endif
