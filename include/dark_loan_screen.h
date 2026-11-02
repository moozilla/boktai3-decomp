#ifndef GUARD_DARK_LOAN_SCREEN_H
#define GUARD_DARK_LOAN_SCREEN_H

#include "display_resource.h"
#include "scene_input_widget.h"
#include "contact_swarm_sprite.h"

struct DarkLoanScreen {
    u8 unk_00[0x18];
    struct DisplayResourceHeader resource;
    struct DisplayResourceHeader secondaryResource;
    struct SceneInputWidget background;
    struct SceneInputWidget text;
    struct SceneInputWidget rateDigit;
    struct SceneInputWidget decimalDigits[2];
    struct SceneInputWidget percent;
    struct SceneInputWidget upperDigits[4];
    struct SceneInputWidget middleDigits[4];
    struct SceneInputWidget lowerRateDigit;
    struct SceneInputWidget lowerDigits[4];
    struct SceneInputWidget label;
    struct SceneInputWidget decorations[2];
    struct ContactSwarmSprite sprite;
    u8 animation[0x1C];
    u8 unknown8E0[4];
    void *procedureData;
    u32 callbackPc;
    u16 rate;
    u8 unknown8EE[2];
    s32 valueD, valueE;
    u8 unknown8F8[0xE];
    u16 displayMode;
    u8 unknown908[7];
    u8 state;
    u32 snapshot[8];
    void (*callback)(void);
};

void DarkLoan_BuildDisplay(struct DarkLoanScreen *, s32);

#endif
