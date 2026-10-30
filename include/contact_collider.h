#ifndef GUARD_CONTACT_COLLIDER_H
#define GUARD_CONTACT_COLLIDER_H

#include "contact_position.h"

struct ContactCollider {
    struct ContactCollider *next; /* intrusive collider-manager list link */
    u8 unknown4[2];
    u16 flags;
    u8 unknown8[2];
    u16 contactFlags; /* 0xA: bit four enables player cost processing */
    u8 unknownC[8];
    struct ContactPosition offset, bounds;
    s16 x, y, z;
    u8 unknown2A[0xA];
    u32 statRewardFlags; /* 0x34: selects player hundredths gain on contact (tentative) */
    u32 reactionFlags;
    u16 unknown3C, cost;
    u16 responseParameter; /* 0x40: copied to actor response state (tentative) */
    u8 direction;
    u8 costFlags;      /* 0x43: bit three latches quarter-cost adjustment */
    u16 responseValue; /* 0x44: copied to actor response state (tentative) */
    u8 responseFlags;  /* 0x46: edge-triggered player contact gates (tentative) */
    u8 unknown47[9];
    void *owner; /* 0x50: callback owner */
};

#endif
