#ifndef GUARD_SOLAR_BANK_PANEL_H
#define GUARD_SOLAR_BANK_PANEL_H
#include "global.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite.h"
/* Solar bank panel layout inferred from static animation and amount accesses.
 * Balance/transfer interpretations remain tentative. */
struct SolarBankIndicatorAnimation {
    u8 data[0x1C];
};
struct SolarBankPanel {
    u8 unk0[0x18];
    struct AnimSpriteResource labelData;
    struct AnimSpriteResource animData;
    struct AnimSprite label;
    struct AnimSprite prompt;
    struct AnimSprite controls[2]; /* interest rate digits */
    struct AnimSprite percentSign;
    struct AnimSprite balanceDigits[4];
    struct AnimSprite transferDigits[4];
    struct AnimSprite transferLabel;
    struct AnimSprite firstMarker;
    struct AnimSprite secondMarker;
    struct DisplaySequenceSprite sprites[2];
    struct SolarBankIndicatorAnimation indicators[2];
    void *layerMap;
    void *paletteResource;
    const void *text;
    s32 transferAmount;
    s32 balance;
    u32 completionScript;
    u16 blinkPhase, holdFrames, repeatSpeed, animationTimer, transferDirection;
    u16 selection;
    u16 timer;
    u8 paletteState;
    u8 unk70F;
    u16 promptPalette[16]; /* 0x710: also the saved copy of the palette bank */
    void (*state)(void);
};
#endif
