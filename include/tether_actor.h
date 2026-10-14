#ifndef GUARD_TETHER_ACTOR_H
#define GUARD_TETHER_ACTOR_H
#include "contact_swarm.h"
struct TetherSegment {
    u32 flags;
    u8 unknown4[12];
    u8 priority, unknown11[7];
    struct ContactPosition position;
    u8 unknown20[8];
};
/* Tentative identity: world actor with seven visible player-tether segments. */
struct TetherActor {
    u8 unknown0[0x24];
    u16 palette[16];
    u8 unknown44[12];
    struct ContactPosition position;
    u8 unknown58[0x208];
    u8 heading, unknown261[3];
    struct TetherSegment segments[7];
    u16 paletteIndex;
    s16 shakeX;
    u8 unknown380[2];
    s16 shakeZ;
    u8 unknown384[4];
    u16 shakeFrame;
    u8 unknown38A[0x11];
    u8 auxiliaryActive;
    struct ContactSwarmSprite auxiliarySprite;
    struct ContactSwarmGraphics auxiliaryGraphics;
    u8 unknown3E4[0xA0];
};
/* Additional player view: these fields are distinct from player.h's facing/actionMode. */
struct TetherPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
    u8 unknown38[0x36A];
    u8 heading, side;
    u8 unknown3A4[0xB3];
    u8 tetherMode;
};
extern struct TetherPlayer *gUnk_02000580[4];
extern const s16 gUnk_086149C4[];
extern u32 gUnk_03005308;
void sub_08217DE0(void *, void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08214584(void *, s32, s32);
#endif
