#ifndef FOUNDRY_REGION_ACTOR_H
#define FOUNDRY_REGION_ACTOR_H
#include "global.h"
/* Tentative region-aware sprite pool; category identity remains unknown. */
struct RegionActor {
    u32 flags;
    u8 unknown4[0x44];
    u32 overlay[0xBC / 4];
    u8 unknown104[2];
    s16 kind;
    u32 behaviorFlags;
    u8 unknown10C[4];
    u8 overlayActive;
    u8 unknown111[3];
    s32 region;
    u8 resource[1];
};
struct RegionActorPool {
    u8 unknown0[0x20];
    struct RegionActor *actors[32];
    u32 used;
    u8 cursor;
};
#endif
