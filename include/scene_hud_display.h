#ifndef GUARD_SCENE_HUD_DISPLAY_H
#define GUARD_SCENE_HUD_DISPLAY_H
#include "scene_hud_map.h"
#include "sun_clock.h"
#include "display_animation.h"
#include "scene_input.h"
#include "contact_actor.h"
#include "scene_vec.h"
struct SceneHudControlAnimation {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
};
struct SceneHudDigitView {
    u8 unknown0[0x20];
    u8 resource[0x12E8];
    struct SceneInputWidget label, digits[4];
};
struct SceneHudWidgetMotion {
    struct SceneVec from, to;
    s16 bendX, bendY;
    u8 unknown14[4];
    u16 timer, duration;
    u8 active, unknown1D[3];
};
/* View of the input grid at SceneHudState + 0x4384; rows overlap auxiliary selectors. */
struct SceneHudActionGrid {
    u8 flags;
    u8 unknown1[0xB];
    u8 cells[9][4];
};
struct SceneHudTertiaryOption {
    u8 slot;
    s8 icon;
    u8 unknown2[2];
};
struct SceneHudPrimaryOption {
    s8 value;
    s8 icon; /* above five: drawn as an attribute icon directly */
    u8 unknown2[2];
};
struct SceneHudSecondaryOption {
    s8 value;
    u8 unknown1[7];
};
/* Scene HUD owner: offsets established by 081651B0, 081658C8 and its initializer 08178FC8. */
struct SceneHudState {
    u8 unknown0[0x18];
    u32 panelResource; /* 0x18: passed to panel initialization */
    u8 unknown1C[0x10];
    u32 panelConfig; /* 0x2C: passed to panel initialization */
    u8 unknown30[0xC];
    const void *actionTextScript;        /* 0x3C */
    const u8 *timedMessageScript;        /* 0x40 */
    const void *optionAScript;           /* 0x44: -a script pointer, meaning tentative */
    const void *unlockTextScript;        /* 0x48 */
    const void *actionDescriptionScript; /* 0x4C */
    u8 unknown50[4];
    const void *optionUpperAScript; /* 0x54: -A */
    const void *optionLScript;      /* 0x58: -l */
    const void *optionUpperEScript; /* 0x5C: -E */
    s32 optionEValue;               /* 0x60: -e */
    u8 widgetResource[0x40];        /* 0x64 */
    u8 motionResource[0x20];        /* 0xA4: keyframe resource */
    u8 unknownC4[0x40];
    u8 actionResultResources[2][0x20]; /* 0x104: normal and bonus keyframes */
    u8 controlAnimationResource[0x20]; /* 0x144 */
    u8 comparisonResource[0x20];       /* 0x164 */
    u8 unknown184[0xA12 - 0x184];
    u16 savedMenuMode; /* 0xA12: restored after page fade */
    u8 unknownA14[0xA3F - 0xA14];
    u8 actionOwnerIndex;        /* 0xA3F: index into party actor pointers */
    struct Player *actionOwner; /* 0xA40 */
    u8 unknownA44[8];
    u16 pageFadeTimer;       /* 0xA4C */
    u16 actionTransferDelay; /* 0xA4E: frames after icon transfer */
    u8 unknownA50[4];
    void *pageCallback;      /* 0xA54 */
    u8 comparisonModifier;   /* 0xA58: modifier-key comparison preview */
    u8 auxiliaryToggle;      /* 0xA59: returned by panel toggle */
    u16 creationParameter;   /* 0xA5A */
    u16 initializationState; /* 0xA5C */
    u8 controlMode;          /* 0xA5E: modes above two suppress controls */
    s8 previousPage;         /* 0xA5F: initialized to -1 */
    u8 unknownA60[4];
    u16 meterPulseFrame; /* 0xA64: 60-frame HUD meter pulse */
    u8 unknownA66[8];
    u16 timedStateParameter;            /* 0xA6E: sixth argument to 081781B4 */
    u16 timedStateStart;                /* 0xA70: negative input clamps to zero */
    u16 timedStateValue;                /* 0xA72 */
    u16 timedStateTimer;                /* 0xA74: initialized to 300 */
    u8 timedStateKind, timedStatePhase; /* 0xA76: phase zero or six */
    u8 unknownA78[8];
    u16 pulseFrame; /* 0xA80: wraps at 128 */
    u8 unknownA82[0x12];
    u8 panelInitialized; /* 0xA94: cleared during gameplay HUD resource setup (tentative) */
    u8 unknownA95[7];
    struct SceneHudControlAnimation controlAnimation; /* 0xA9C */
    u8 unknownABC[0x41];
    u8 actionRefreshRequested; /* 0xAFD */
    u8 unknownAFE[2];
    u8 actionCueInitialized, actionCueSuppressed; /* 0xB00 */
    u8 previousCueGroup, previousCueSelection;    /* 0xB02 */
    u8 unknownB04[0x16C];
    u8 primaryVisible;           /* 0xC70 */
    u8 primarySlideTimer;        /* 0xC71: remaining three-pixel slide steps */
    s8 primarySelection;         /* 0xC72 */
    u8 primaryBlinkTimer;        /* 0xC73: 60-frame icon blink */
    s16 *primarySelectionOutput; /* 0xC74 */
    struct SceneHudPrimaryOption primaryOptions[3];
    u8 unknownC84[4];
    struct SceneInputWidget primaryWidgets[3]; /* 0xC88 */
    s8 secondaryFirstValue;  /* 0xDA8: first value of stride-eight option records */
    s8 secondaryUnlockIndex; /* 0xDA9: indexed with stride eight */
    u8 secondaryEntryEnabled, secondaryEntryIcon; /* 0xDAA: indexed with stride eight */
    s32 secondaryActionValue;                     /* 0xDAC: indexed with stride eight */
    u8 unknownDB0[0x78];
    struct SceneInputWidget secondaryWidgets[3]; /* 0xE28 */
    s8 selection;                                /* 0xF48 */
    u8 unknownF49;
    s8 tertiarySelection;  /* 0xF4A */
    u8 tertiaryVisible;    /* 0xF4B */
    u8 tertiarySlideTimer; /* 0xF4C */
    u8 unknownF4D[3];
    struct SceneHudTertiaryOption tertiaryOptions[8]; /* 0xF50 */
    struct SceneInputWidget tertiaryWidgets[3];       /* 0xF70 */
    struct SceneInputWidget extraWidgetsA[3];         /* 0x1090 */
    struct SceneInputWidget extraWidgetsB[3];         /* 0x11B0 */
    struct SceneInputWidget extraWidgetsC[3];         /* 0x12D0 */
    u8 unknown13F0;
    u8 secondaryInputEnabled; /* 0x13F1 */
    u8 secondaryCount;        /* 0x13F2 */
    u8 secondarySlideTimer;   /* 0x13F3 */
    u8 unknown13F4[0x20];
    u8 mode; /* 0x1414: snapshot of gSceneHudMode */
    u8 unknown1415;
    u8 pageEnabled[10]; /* 0x1416: script -o visibility list */
    struct SceneInputWidget
        categoryWidgets[30]; /* 0x1420: ten controls, eight action icons, twelve unlock icons */
    u8 unknown1F60[0xC0];
    struct SceneInputWidget modeWidget; /* 0x2020 */
    u8 unknown2080[0x4E0];
    struct SceneInputWidget actionPanelWidgets[3]; /* 0x2560 */
    u8 unknown2680[0x22];
    u16 actionFeedbackY; /* 0x26A2: transient action feedback animation */
    u8 unknown26A4[0x39C];
    struct SceneHudControlAnimation actionAnimation; /* 0x2A40 */
    u8 unknown2A60[0x40];
    struct SceneInputWidget motionWidgets[15]; /* 0x2AA0: extent bounded by valueWidgets */
    struct SceneInputWidget valueWidgets[2];   /* 0x3040, 0x30A0: widget indices 75 and 76 */
    u8 unknown3100[0x240];
    struct SceneInputWidget comparisonIndicators[2]; /* 0x3340 */
    struct SceneInputWidget statMarkers[3][3];       /* 0x3400: base, bonus, delta */
    u8 unknown3760[0xA80];
    struct SceneInputWidget transferWidgets[2]; /* 0x41E0: hidden after action transfer */
    u8 statusRefreshRequested;                  /* 0x42A0 */
    u8 pageRepeatTimer;                         /* 0x42A1: held shoulder-button repeat */
    u8 clockSelection;                          /* 0x42A2: base time, time A or time B */
    u8 unknown42A3;
    struct SceneInputWidget comparisonActionWidget; /* 0x42A4 */
    struct SceneInputWidget comparisonUnlockWidget; /* 0x4304 */
    u16 comparisonPalette[16];                      /* 0x4364 */
    u8 actionInput[3];  /* 0x4384: action-grid input state, extent tentative */
    u8 actionCursor;    /* 0x4387: position lookup index */
    u8 actionGridGroup; /* 0x4388: values above one select unlock confirmation */
    u8 unknown4389;
    u8 actionSelection; /* 0x438A: selected action lookup */
    u8 unknown438B[0x5E];
    u8 binaryChoice; /* 0x43E9: left/right toggles; A confirms, B cancels */
    u8 unknown43EA[2];
    struct SceneHudWidgetMotion widgetMotions[1]; /* 0x43EC: indexed, count unconfirmed */
    u8 unknown440C[0x20];
    struct ContactPosition actionTargets[15]; /* 0x442C: indexed alongside motionWidgets */
    u8 unknown44A4[0x3BC];
    u8 actionCategory;      /* 0x4860: action lookup category */
    u8 actionTransferBonus; /* 0x4861: bonus animation/palette selection (tentative) */
    u8 selectedStat,
        appliedStatIncrease; /* 0x4862: base-stat bonus selection and clamped increase */
    u8 unknown4864[2];
    s16 appliedStatB; /* 0x4866: nonnegative stat B change */
    u8 unknown4868[0x30];
    s8 comparisonProfiles[0x14]; /* 0x4898: profile index per party member, extent tentative */
    u8 comparisonValues[3];      /* 0x48AC: capped combined stat values */
    u8 unknown48AF;
    s16 comparisonDeltas[3];  /* 0x48B0: signed bar adjustments after offsets */
    s16 comparisonOffsets[3]; /* 0x48B6: signed bar adjustments */
    u8 pageInputCooldown;     /* 0x48BC */
    u8 unknown48BD[0xB];
    s32 timedMessageWindow; /* 0x48C8 */
    u8 unknown48CC[0x19];
    u8 pageReturnToParent; /* 0x48E5 */
    u8 selectedQuantity;   /* 0x48E6: multiplied by valueRate */
    u8 unknown48E7;
    u8 quantityArrowFrame; /* 0x48E8: arrow keyframe base, 0x13 up and 0x16 down */
    u8 quantityArrowTimer; /* 0x48E9: arrow animation, wraps at sixteen */
    u16 valueRate;         /* 0x48EA */
    u16 balanceLimitTimer; /* 0x48EC: frames left of the over-limit warning */
    u16 balanceLimitShown; /* 0x48EE: over-limit message already shown */
    u8 unknown48F0[0x4988 - 0x48F0];
    struct SceneInputWidget ownerWidgets[2]; /* 0x4988: removed by owner destructor */
    u8 unknown4A48[0x28];
    struct SceneInputWidget ownerAuxWidgets[6]; /* 0x4A70 */
    u8 unknown4CB0[0x28];
    u8 initializationFlag; /* 0x4CD8: cleared by both initializers */
    u8 unknown4CD9[0x4D90 - 0x4CD9];
    u8 equippedIconSlots[8]; /* 0x4D90: inventory slot for each visible equipped icon */
};
struct SceneHudNumericView {
    u8 unknown0[0x50C];
    s32 value50C;
};
u32 SunClock_GetBaseHour(void);
u32 SunClock_GetBaseMinute(void);
u32 SceneInput_TestGridFlags(const void *, u32);
void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *, u32);
void SceneHud_SetTimedState(u32 unused, s32 start, u32 value, u32 kind, u32 phaseEnabled,
                            u32 parameter);
#endif
