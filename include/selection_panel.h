#ifndef GUARD_SELECTION_PANEL_H
#define GUARD_SELECTION_PANEL_H
#include "character_preview_actor.h"
#include "scene_input.h"
struct SelectionPanelPaletteCycle {
    u16 colors[16], frame;
    u8 unknown22[2];
};
struct SelectionPanel {
    struct Task task;
    void *backgroundResource, *paletteResource;
    struct SceneInputWidget widgets[10];
    struct SceneInputResource auxiliaryResource, widgetResource;
    u16 entryIds[10], cachedIds[10];
    void *widgetScript, *auxiliaryScript;
    u16 widgetPalette[16];
    struct SelectionPanelPaletteCycle paletteCycle;
    struct CharacterPreviewActor preview;
    void (*update)(struct SelectionPanel *);
    void *sourceScript;
    const u8 *contextScript;
    const void *descriptionText;
    void *nameScript;
    u8 unknownCF8[8];
    u32 selection;
    u8 state;
    s8 lastDescription;
    u8 previousSelection, pulseFrame;
    s32 windows[7];
    s8 column;
    u8 repeat;
    s8 page;
    u8 minimumPage, maximumPage, maximumColumn, heldKeys, unknownD2B;
    u8 entering, unknownD2D;
    u16 timer;
    u32 cancelScript, selectionScript;
};
struct SelectionPanelScriptArgs {
    u32 count : 16, reserved : 16;
    s32 *values;
};
void SelectionPanel_ClearTextWindows(struct SelectionPanel *panel);
void SelectionPanel_InitEntryIds(struct SelectionPanel *panel);
void SelectionPanel_InitializeWidgets(struct SelectionPanel *panel);
void SelectionPanel_PositionWidgets(struct SelectionPanel *panel);
void SelectionPanel_InitPreview(struct SelectionPanel *panel);
void SelectionPanel_HideDisplayObjects(struct SelectionPanel *panel);
void SelectionPanel_RemoveWidgets(struct SelectionPanel *panel);
void SelectionPanel_HandleInput(struct SelectionPanel *panel);
void SelectionPanel_InitializeTextState(struct SelectionPanel *panel);
void SelectionPanel_UpdateDescription(struct SelectionPanel *panel);
s32 SelectionPanel_GetSelection(struct SelectionPanel *panel);
void SelectionPanel_Close(struct SelectionPanel *panel);
void SelectionPanel_CommitSelection(struct SelectionPanel *panel);
void SelectionPanel_RunSelectionScript(struct SelectionPanel *panel);
void SelectionPanel_RunCancelScript(struct SelectionPanel *panel);
void SelectionPanel_OnCloseRequested(struct SelectionPanel *panel);
void SelectionPanel_UpdateConfirmTransition(struct SelectionPanel *panel);
void SelectionPanel_UpdateCancelTransition(struct SelectionPanel *panel);
void SelectionPanel_OnTransitionIdle(void);
void SelectionPanel_UpdateInput(struct SelectionPanel *panel);
s32 SelectionPanel_Update(struct SelectionPanel *panel);
s32 SelectionPanel_OnDestroy(struct SelectionPanel *panel);
s32 SelectionPanel_InitializeFromScript(struct SelectionPanel *panel);
struct SelectionPanel *SelectionPanel_Create(void);
#endif
