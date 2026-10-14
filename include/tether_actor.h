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
struct TetherPaletteView {
    u8 unknown0[0x18];
    const u16 *palette;
    u8 unknown1C[0x78];
};
struct TetherActor {
    u8 unknown0[0x1A];
    u8 kind, unknown1B[9];
    u16 palette[16];
    u8 unknown44[12];
    struct ContactPosition position;
    u8 unknown58[0x34];
    union {
        struct DisplayAnimation animation;
        struct TetherPaletteView colors;
    } display;
    u8 unknown120[0x140];
    u8 heading, unknown261[3];
    struct TetherSegment segments[7];
    u16 paletteIndex;
    s16 shakeX;
    u8 unknown380[2];
    s16 shakeZ;
    u8 unknown384[4];
    u16 shakeFrame;
    u16 glowMode, glowFrame;
    u16 appliedPalette, paletteOverrideTimer, unknown392;
    u16 paletteBlend;
    u16 effectFrame, soundCooldown;
    u8 moving;
    u8 auxiliaryActive;
    struct ContactSwarmSprite auxiliarySprite;
    struct ContactSwarmGraphics auxiliaryGraphics;
    u8 effectsSuppressed;
    u8 unknown3E5[0x89];
    u16 blendHold;
    u8 unknown470[2];
    u8 blendRequested, unknown473[0x11];
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
