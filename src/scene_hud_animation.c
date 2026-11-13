#include "global.h"
#include "scene_hud_display.h"
#include "input.h"

void MenuDisplay_UpdateActionPrompt(void *, int);

void InventoryPanel_SetEntryRow(void *, u32, u32, u32, u32, u32);

u32 InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);

void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);

int SceneInput_NavigateGridKeys(void *, int);

void Hud_SaveGridSelection(void *);

void SceneHud_RefreshEquippedActionIcons(void *);

void PauseMenu_RefreshFruitAndUnlockIcons(void *);

void SceneHud_SetActionCursorPosition(void *, u32);

void PauseInventory_ReturnToEquipmentSelection(void *);

void SolarFruit_ToggleEquipSlot(void *);

u8 GameState_ToggleUnlockEquipSlot(void *);

void SceneHud_ShowSelectionDescription(void *);

void SceneHud_UpdateActionGridInput(struct SceneHudState *p)
{
    u32 initialGroup = p->actionGridGroup;
    s32 unlockGroup = 1;
    s32 moved;
    void *grid;
    u8 *cursor;
    if (initialGroup <= 1)
        unlockGroup = 0;
    grid = p->actionInput;
    moved = SceneInput_NavigateGridKeys(grid, 0xF0);
    if (moved) {
        SceneHud_SetActionCursorPosition(&p->categoryWidgets[5].position,
                                         *(cursor = &p->actionCursor));
        SceneHud_ShowSelectionDescription(p);
        Hud_SaveGridSelection(grid);
        {
            u32 selected = *cursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            MenuDisplay_UpdateActionPrompt(p, group);
        }
        if (*cursor > 7)
            InventoryPanel_DrawStatComparison(p, 1, 0, 0, 0, 0);
        else
            InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, 0);
    } else if (gPadState[1] & 1) {
        if (!unlockGroup)
            SolarFruit_ToggleEquipSlot(grid);
        else if (GameState_ToggleUnlockEquipSlot(grid)) {
            u32 result = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, moved);
            InventoryPanel_SetEntryRow(p, 3, 14, result, moved, 0x47);
        }
        PauseMenu_RefreshFruitAndUnlockIcons(p);
        {
            u32 selected = p->actionCursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            MenuDisplay_UpdateActionPrompt(p, group);
        }
        SceneHud_RefreshEquippedActionIcons(p);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToEquipmentSelection(p);
    }
}

void Sound_PlaySong(u32);

void SceneHud_ConfigureActionGrid(void *, u32, u32);

void SceneHud_UpdateActionTransferSelection(struct SceneHudState *);

void Hud_SetInteractionCallback(void *, void (*)(void), int);

void SceneHud_UpdateActionChoice(struct SceneHudState *p)
{
    u8 *q = p->actionInput;
    u8 *cursor;
    u32 mode;
    u8 value;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowSelectionDescription(p);
    }
    cursor = &p->actionCursor;
    value = *cursor;
    mode = 1;
    if (value <= 7)
        mode = 0;
    MenuDisplay_UpdateActionPrompt(p, mode);
    if (gPadState[1] & 2) {
        PauseInventory_ReturnToEquipmentSelection(p);
        Sound_PlaySong(0xde);
    } else if (1 & gPadState[1]) {
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        p->categoryWidgets[6].flags &= ~1;
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1680, p->actionSelection);
        if (*cursor <= 7)
            SceneHud_ConfigureActionGrid(q, 0, 1);
        else
            SceneHud_ConfigureActionGrid(q, 0, 2);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionTransferSelection, 1);
    }
}

#define SceneInput_SetWidgetPalette PauseMenu_SetSpritePalette

s32 GameState_GetUnlockEntry(s32);

s32 SolarFruit_GetInventorySlot(u32);

void PauseMenu_SetSpritePalette(void *, u16);

void SceneHud_LoadActionMotion(struct SceneHudState *p, u32 index, u32 source, u32 target, u32 kind)
{
    struct SceneInputWidget *w = &p->motionWidgets[index];
    w->flags &= ~1;
    if (kind == 0) {
        void *data = p->motionResource;
        u16 frame = SolarFruit_GetInventorySlot(source) + 0xe7;
        Anim_LoadKeyframe(w, data, frame, 1);
    } else if (kind == 1) {
        void *data = p->motionResource;
        u16 frame = GameState_GetUnlockEntry(source - 8) + 0xd9;
        Anim_LoadKeyframe(w, data, frame, 1);
    } else
        return;
    SceneHud_SetActionCursorPosition(&w->position, source);
    SceneHud_SetActionCursorPosition(&p->actionTargets[index], target);
    PauseMenu_SetSpritePalette(w, p->categoryWidgets[source + 10].paletteBank);
}

s32 MenuDisplay_CanSwapCategoryEntries(void *, u32, u32, u32);

void MenuDisplay_BeginActionTransfer(void *, u32);

void SceneHud_FinishActionTransfer(struct SceneHudState *);

void SceneHud_UpdateActionChoice(struct SceneHudState *);

void Hud_RestoreGridSelection(void *);

void Hud_SetGridPosition(void *, u32, u32);

// confidence=low: confirms transfers between fruit or unlock grid entries.

void SceneHud_UpdateActionTransferSelection(struct SceneHudState *p)
{
    u8 *grid = p->actionInput;
    u8 *selected;
    u32 value, mode;
    if (SceneInput_NavigateGridKeys(grid, 0xf0)) {
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowSelectionDescription(p);
    }
    selected = &p->actionCursor;
    value = *selected;
    mode = 1;
    if (value <= 7)
        mode = 0;
    MenuDisplay_UpdateActionPrompt(p, mode);
    if (gPadState[1] & 1) {
        u8 *source;
        u32 current;
        if (p->actionInput[1] <= 1) {
            current = *selected;
            source = &p->actionSelection;
            if (!MenuDisplay_CanSwapCategoryEntries(p, current, *source, 0)) {
                Sound_PlaySong(0x192);
                return;
            }
            Sound_PlaySong(0x38a);
            {
                s32 slot = SolarFruit_GetInventorySlot(*source);
                if (slot != -1)
                    SceneHud_LoadActionMotion(p, 0, *source, *selected, 0);
                if (SolarFruit_GetInventorySlot(*selected) != -1)
                    SceneHud_LoadActionMotion(p, 1, *selected, *source, 0);
                MenuDisplay_BeginActionTransfer(p, 0);
            }
        } else {
            current = *selected;
            source = &p->actionSelection;
            if (!MenuDisplay_CanSwapCategoryEntries(p, current, *source, 1)) {
                Sound_PlaySong(0x192);
                return;
            }
            if (GameState_GetUnlockEntry(*source - 8))
                SceneHud_LoadActionMotion(p, 0, *source, *selected, 1);
            if (GameState_GetUnlockEntry(*selected - 8))
                SceneHud_LoadActionMotion(p, 1, *selected, *source, 1);
            MenuDisplay_BeginActionTransfer(p, 1);
        }
        p->categoryWidgets[p->actionSelection + 10].flags |= 1;
        p->categoryWidgets[p->actionCursor + 10].flags |= 1;
        Hud_SetInteractionCallback(p, SceneHud_FinishActionTransfer, 1);
    } else if (gPadState[1] & 2) {
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
        SceneHud_ConfigureActionGrid(grid, 0, 0);
        p->categoryWidgets[6].flags |= 1;
        Hud_RestoreGridSelection(grid);
        Hud_SetGridPosition(grid, p->actionInput[1], p->actionInput[2]);
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, *selected);
        Sound_PlaySong(0xde);
    }
}

void PauseMenu_SmoothPoint(void *, void *, s32);

void MenuDisplay_InterpolateActivePairPositions(void *, s32);

void sub_0822EB14(u32, u32);

void sub_0822E764(u32, u32);

void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);

void SceneHud_FinishActionTransfer(struct SceneHudState *p)
{
    u16 *timer = &p->actionTransferDelay;
    if (!*timer)
        Sound_PlaySong(0x38a);
    (*timer)++;
    if (*timer <= 11) {
        s32 n = 12 - *timer;
        PauseMenu_SmoothPoint(&p->motionWidgets[0].position, &p->actionTargets[0], n);
        PauseMenu_SmoothPoint(&p->motionWidgets[1].position, &p->actionTargets[1], n);
        MenuDisplay_InterpolateActivePairPositions(p, n);
    } else {
        u32 *q = &p->motionWidgets[0].flags;
        u32 v = *q, m = 1;
        *q = v | m;
        p->motionWidgets[1].flags |= m;
        p->transferWidgets[0].flags |= m;
        p->transferWidgets[1].flags |= m;
        if (p->actionInput[1] <= 1)
            sub_0822EB14(p->actionSelection, p->actionCursor);
        else
            sub_0822E764(p->actionSelection - 8, p->actionCursor - 8);
        PauseMenu_RefreshFruitAndUnlockIcons(p);
        p->categoryWidgets[6].flags |= 1;
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
        SceneHud_ConfigureActionGrid(p->actionInput, 0, 0);
        SceneHud_ShowSelectionDescription(p);
    }
}

/* Tentative semantic names; field offsets are established by static accesses. */

struct MotionSequence {
    u8 unknown0[0x4388];
    u8 selectionA, selectionB, selection;
};

int PauseMenu_UpdateBinaryChoice(void *);

void sub_0822E86C(u32);

void SolarFruit_SortInventorySlots(void *);

void MotionSequence_ConfirmReset(struct MotionSequence *p)
{
    s32 r = PauseMenu_UpdateBinaryChoice(p);
    if (r == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToEquipmentSelection(p);
    } else if (r == 1) {
        Sound_PlaySong(0xDD);
        sub_0822E86C(0);
        SolarFruit_SortInventorySlots(0);
        p->selectionA = 0;
        p->selectionB = 0;
        p->selection = 0;
        PauseInventory_ReturnToEquipmentSelection(p);
    }
}

void Hud_SaveSecondaryGridSelection(void *);

void PauseMenu_AddSpriteFlags(void *, u32, u32);

void PauseMenu_ClearSpriteFlags(void *, u32, u32);

void PauseMenu_OpenPrompt(u32, u32, u32);

void PauseMenu_SetConfirmationIndicator(void *, u32);

void PauseMenu_HidePromptIcons(void *);

static inline u8 *SceneHud_GetActionInput(u8 *p)
{
    return p + 0x4384;
}

void SceneHud_UpdateSecondaryActionSelection(u8 *p)
{
    if (gPadState[1] & 1) {
        u8 *grid;
        u8 *selected;
        Sound_PlaySong(0xdd);
        grid = p + 0x4384;
        Hud_SaveSecondaryGridSelection(grid);
        SceneHud_SetActionCursorPosition(p + 0x15c0, *(selected = p + 0x4387));
        if (*selected == 39) {
            PauseMenu_AddSpriteFlags(p, 5, 1);
            PauseMenu_AddSpriteFlags(p, 59, 1);
            PauseMenu_OpenPrompt(*(u32 *)(p + 0x5c), 4, 0);
            PauseMenu_SetConfirmationIndicator(p, 1);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
            PauseMenu_HidePromptIcons(p);
            Hud_SetInteractionCallback(p, MotionSequence_ConfirmReset, 1);
            return;
        }
        if (*selected == 32) {
            Hud_SetInteractionCallback(p, SceneHud_UpdateActionGridInput, 1);
            SceneHud_ConfigureActionGrid(grid, 0, 0);
            MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
            Hud_SetGridPosition(grid, 0, 0);
            SceneHud_ShowSelectionDescription(p);
        } else if (*selected == 38) {
            u8 *g2;
            PauseMenu_RefreshFruitAndUnlockIcons(p);
            *(u32 *)(p + 0x1668) |= 1;
            Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
            g2 = SceneHud_GetActionInput(p);
            Hud_SetGridPosition(g2, 0, 0);
            MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
            SceneHud_ConfigureActionGrid(g2, 0, 0);
            SceneHud_ShowSelectionDescription(p);
        }
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        Hud_SetGridPosition(((struct SceneHudState *)p)->actionInput, 0, 0);
        SceneHud_SetActionCursorPosition(p + 0x1620, *(selected = p + 0x4387));
        {
            u32 value = *selected;
            u32 prompt = 1;
            if (value <= 7)
                prompt = 0;
            MenuDisplay_UpdateActionPrompt(p, prompt);
        }
    } else if (SceneInput_NavigateGridKeys(p + 0x4384, 0xc0)) {
        u8 *selected = p + 0x4387;
        {
            u32 value = *selected;
            u32 prompt = 1;
            if (value <= 7)
                prompt = 0;
            MenuDisplay_UpdateActionPrompt(p, prompt);
        }
        SceneHud_SetActionCursorPosition(p + 0x15c0, *selected);
    }
}
