#ifndef GUARD_RESOURCE_STATION_H
#define GUARD_RESOURCE_STATION_H
#include "game_state.h"
#include "ram.h"
/* Tentative identity: a station that transfers stored resource into player+428. */
struct ResourceStationPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct ResourceStationSprite {
    u32 flags;
    u8 unknown4[0x18];
    struct ResourceStationPosition position;
    u8 unknown24[8];
};
struct ResourceStationDigit {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    struct ResourceStationPosition position;
    u8 unknown28[0x38];
};
struct ResourceStationPlayer {
    u8 unknown0[0x1C];
    u32 stateTag, flags; /* stateTag interpretation is tentative; active value is1 */
    u8 unknown24[12];
    struct ResourceStationPosition position;
    u8 unknown38[0x3E0];
    u8 actionMode;
    u8 unknown419[0xF];
    u16 resource, resourceMaximum;
    u8 unknown42C[0x2B];
    u8 action, actionPhase;
    u8 unknown459[0xB];
    u8 busy;
    u8 unknown465[0x87];
    const u16 *input;
};
struct ResourceStation {
    u8 task[0x18];
    struct ResourceStationSprite sprite;
    u8 graphics[0x1C];
    u8 digitGraphics[0x20];
    struct ResourceStationDigit digits[4];
    u8 collider[0x54];
    struct ResourceStationPosition triggerPosition;
    struct ResourceStationPlayer *player;
    u16 scriptHandle;
    u8 state, animationFrame, nearby, digitsVisible;
    u16 interactionVariant;
};
extern u16 gUnk_02000534;
extern struct ResourceStationPosition gUnk_03005418;
void AnimData_SelectPaletteBank(void *, s32);
#endif
