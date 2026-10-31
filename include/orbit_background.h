#ifndef GUARD_ORBIT_BACKGROUND_H
#define GUARD_ORBIT_BACKGROUND_H
#include "contact_swarm.h"
#include "game_state.h"
#include "render.h"
#include "ram.h"
/* Tentative identity: BG1 image positioned from an orbit around the selected player. */
struct OrbitBackground {
    u8 unknown0[0x18];
    void *resource;
    u16 id;
    u8 playerIndex, enabled;
    struct ContactPosition target, position;
    u16 angle;
    s16 radius;
    u16 blendFrame;
    u8 holdCamera, specialMode;
};
struct OrbitBackgroundPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
    u8 unknown38[0x41F];
    u8 transition;
    u8 unknown458[0x50];
    struct ContactPosition target;
    u8 unknown4B0[6];
    s16 targetActive;
    u8 unknown4B8[0x63A];
    u16 heightScale;
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
extern struct OrbitBackgroundPlayer *gPlayers[4];
extern const s16 gUnk_086149C4[];
void OrbitBackground_ResetOrbit(struct OrbitBackground *);
void OrbitBackground_UpdateVisibility(struct OrbitBackground *);
void OrbitBackground_UpdatePosition(struct OrbitBackground *);
s32 OrbitBackground_Tick(struct OrbitBackground *);
void OrbitBackground_LoadResource(struct OrbitBackground *);
s32 Div(s32, s32);
#endif
