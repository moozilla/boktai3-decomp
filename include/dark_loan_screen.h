#ifndef GUARD_DARK_LOAN_SCREEN_H
#define GUARD_DARK_LOAN_SCREEN_H
#include "scene_input.h"
#include "display_resource.h"
#include "contact_swarm_sprite.h"
#include "player.h"

enum DarkLoanWidgetIndex {
    DARK_LOAN_BACKGROUND = 0,
    DARK_LOAN_CURSOR = 1,
    DARK_LOAN_RATE = 2,
    DARK_LOAN_DECIMAL_DIGITS = 3,
    DARK_LOAN_PERCENT = 5,
    DARK_LOAN_BALANCE_DIGITS = 6,
    DARK_LOAN_ADJUSTMENT_DIGITS = 10,
    DARK_LOAN_LOWER_RATE = 14,
    DARK_LOAN_AMOUNT_DIGITS = 15,
    DARK_LOAN_LABEL = 19,
    DARK_LOAN_DECORATIONS = 20,
    DARK_LOAN_WIDGET_COUNT = 22
};
struct DarkLoanHighlightPalette {
    u32 header[6];
    u16 colors[16];
};

struct DarkLoanMapHeader {
    u32 header[3], offsets[3];
};
struct DarkLoanScreen {
    struct Task task;
    struct DisplayResourceHeader resource, secondaryResource;
    struct SceneInputWidget widgets[DARK_LOAN_WIDGET_COUNT];
    struct ContactSwarmSprite sprite;
    u8 animation[0x1C];
    struct DarkLoanMapHeader *map;
    void *procedureData;
    u32 callbackPc;
    u16 rate, adjustment;
    s32 confirmScript, exitScript;
    u16 adjustmentState[3], changeFrame, changeKeyframe;
    u16 selection, timer, displayMode, warningTimer, warningShowing, unknown90C;
    u8 unknown90E, highlightPulse;
    u32 snapshot[8];
    void (*callback)(struct DarkLoanScreen *);
};

void DarkLoan_RelocateMapHeaderCopy(struct DarkLoanScreen *panel);
void DarkLoan_EnableAlternateRenderMode(struct DarkLoanScreen *panel);
void DarkLoan_ShowPrompt(struct DarkLoanScreen *panel);
void DarkLoan_ResetBackground(struct DarkLoanScreen *panel);
void DarkLoan_UpdateBalanceDigits(struct DarkLoanScreen *s);
void DarkLoan_UpdateAdjustmentDigits(struct DarkLoanScreen *display);
void DarkLoan_DrawPreview(struct DarkLoanScreen *panel);
void DarkLoan_HideSprites(struct DarkLoanScreen *panel);
void DarkLoan_SetState(struct DarkLoanScreen *panel, void (*v)(struct DarkLoanScreen *));
void DarkLoan_BeginOpening(struct DarkLoanScreen *panel);
void DarkLoan_FinishOpening(struct DarkLoanScreen *panel);
void DarkLoan_BeginAdjustment(struct DarkLoanScreen *panel);
void DarkLoan_BeginClosing(struct DarkLoanScreen *panel);
void DarkLoan_ShowCompletionMessage(struct DarkLoanScreen *panel);
void DarkLoan_BeginAlternateOpening(struct DarkLoanScreen *panel);
void DarkLoan_HideDisplay(struct DarkLoanScreen *panel);
void DarkLoan_UpdateHidden(struct DarkLoanScreen *panel);
void DarkLoan_UpdateOpeningAnimation(struct DarkLoanScreen *panel);
void DarkLoan_ConfirmPreview(struct DarkLoanScreen *panel);
void DarkLoan_UpdateConfirmation(struct DarkLoanScreen *panel);
s32 DarkLoan_GetSelectionGroup(s32 selection);
void DarkLoan_UpdateAdjustment(struct DarkLoanScreen *panel);
void DarkLoan_WaitBeforeConfirmation(struct DarkLoanScreen *panel);
void DarkLoan_WaitAfterConfirm(struct DarkLoanScreen *panel);
void DarkLoan_WaitBeforeFade(struct DarkLoanScreen *panel);
void DarkLoan_CloseAfterDelay(struct DarkLoanScreen *panel);
void DarkLoan_UpdateClosing(struct DarkLoanScreen *panel);
s32 DarkLoan_OnUpdate(struct DarkLoanScreen *panel);
s32 DarkLoan_OnDestroy(struct DarkLoanScreen *panel);
void DarkLoan_StartDisplayScript(struct DarkLoanScreen *panel);
void DarkLoan_InitializeProcedure(struct DarkLoanScreen *panel);
void DarkLoan_BuildDisplay(struct DarkLoanScreen *panel, s32 alternate);
s32 DarkLoan_GetDefaultRate(void);
s32 DarkLoan_InitializeFromScript(struct DarkLoanScreen *panel);
struct DarkLoanScreen *DarkLoan_Create(void);
struct DarkLoanScreen *DarkLoan_CreateHiddenDisplay(u32 rate, s32 valueD, u32 displayMode,
                                                    u32 callbackPc);
void DarkLoan_OpenDisplay(struct DarkLoanScreen *panel);
#endif
