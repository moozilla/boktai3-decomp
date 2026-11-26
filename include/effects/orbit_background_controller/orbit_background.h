#ifndef GUARD_ORBIT_BACKGROUND_H
#define GUARD_ORBIT_BACKGROUND_H
#include "engine/shared/contact_swarm.h"
#include "system/game_state/game_state.h"
#include "engine/shared/render.h"
#include "engine/shared/ram.h"
#include "engine/shared/task.h"
#include "engine/shared/player.h"
/* Tentative identity: BG1 image positioned from an orbit around the selected player. */
struct OrbitBackground {
    struct Task task;
    void *resource;
    u16 id;
    u8 playerIndex, enabled;
    struct ContactPosition target, position;
    u16 angle;
    s16 radius;
    u16 blendFrame;
    u8 holdCamera, specialMode;
};
struct OrbitBackgroundSelection {
    u8 unknown0[0x18];
    s16 selected;
    u8 unknown1A[0x42];
    u8 status[1];
};
/* 0x0200048C */
extern struct OrbitBackground *gOrbitBackground;
extern struct OrbitBackgroundSelection *gLinkSession;
extern const s16 gUnk_086149C4[];
void OrbitBackground_ResetOrbit(struct OrbitBackground *);
void OrbitBackground_UpdateVisibility(struct OrbitBackground *);
void OrbitBackground_UpdatePosition(struct OrbitBackground *);
s32 OrbitBackground_Tick(struct OrbitBackground *);
void OrbitBackground_LoadResource(struct OrbitBackground *);
s32 Div(s32, s32);
#endif
