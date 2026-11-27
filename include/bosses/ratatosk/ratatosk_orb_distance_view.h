#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_RATATOSK_ORB_DISTANCE_VIEW_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_RATATOSK_ORB_DISTANCE_VIEW_H

#include "global.h"
#include "bosses/ratatosk/ratatosk_orb_targets.h"
#include "bosses/ratatosk/scene_distance_entry_layout.h"
#include "engine/math/scene_vec.h"
#include "objects/sun_orb/sun_orb_effect.h"

union RatatoskOrbDistanceViewOrbs {
    struct SceneDistanceEntry linked[4];
    union RatatoskLinkedOrbs orbs; /* confidence=low */
};

typedef char RatatoskOrbDistanceViewOrbsSizeCheck[sizeof(union RatatoskOrbDistanceViewOrbs) == 0x60 ? 1 : -1];

struct RatatoskOrbDistanceView {
    u8 pad0[0x54];
    struct OrbitEffectPosition position; /* confidence=low */
    u8 pad5C[0x34];
    u32 counter; /* confidence=low */
    u8 pad94[0x16];
    u8 animation; /* confidence=low */
    u8 padAB[0x3];
    u8 requested; /* confidence=low */
    u8 input; /* confidence=low */
    u8 active; /* confidence=low */
    u8 padB1[0x13];
    u32 timer; /* confidence=low */
    u8 padC8[0x4];
    u8 direction; /* confidence=low */
    u8 padCD[0xb3];
    s32 sequence; /* confidence=low */
    u32 finished; /* confidence=low */
    u8 pad188[0x1e4];
    struct SceneVec anchor;
    u8 pad374[0xa7c];
    union RatatoskOrbDistanceViewOrbs orbsStorage;
    u8 padE50[0xa];
    s16 completion; /* confidence=low */
    struct OrbitEffectPosition destination; /* confidence=low */
    struct OrbitEffectPosition target; /* confidence=low */
    s16 duration; /* confidence=low */
    u8 padE6E[0x6];
    s16 pending; /* confidence=low */
    u8 padE76[0xe];
    s16 elapsed; /* confidence=low */
    s16 phaseTime; /* confidence=low */
    s16 orbCreated; /* confidence=low */
    s16 playerAngle; /* confidence=low */
    s16 released; /* confidence=low */
    u8 padE8E[0x4];
    s16 oldSlot; /* confidence=low */
    u8 padE94[0x4a];
    s16 repeat; /* confidence=low */
    s16 particles; /* confidence=low */
    s16 reuse; /* confidence=low */
    u8 padEE4[0x2];
    s16 releaseAngle; /* confidence=low */
    u8 padEE8[0x26];
    u16 orbParam; /* confidence=low */
    u8 padF10[0x1c];
    struct OrbitEffectManager *manager;
};

typedef char RatatoskOrbDistanceViewSizeCheck[sizeof(struct RatatoskOrbDistanceView) == 0xf30 ? 1 : -1];

#endif
