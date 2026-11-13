#ifndef GUARD_LINK_PANEL_H
#define GUARD_LINK_PANEL_H
#include "scene_input.h"
struct LinkPanelPaletteCycle {
    u16 colors[16], frame;
    u8 unknown22[2];
};
struct LinkPanelStatus {
    u8 currentStatus, previousStatus, animationTimer;
    u8 unknown3;
};
/* Tentative slot extent: status bytes occupy the final four-byte record.
 * Two slots start at 0x348 and end at the callback at 0xF40. */
struct LinkPanelEntry {
    u8 unknown0[0x5F8];
    struct LinkPanelStatus status;
};
struct LinkPanel {
    struct Task task;
    void *backgroundResource;
    void *paletteResource;
    struct SceneInputWidget animations[7];
    struct SceneInputResource lastEntryResource, entryResource;
    u16 selected[7];
    s16 previous[7];
    u8 unknown31C[8];
    struct LinkPanelPaletteCycle paletteCycle;
    struct LinkPanelEntry entries[2];
    void (*update)(struct LinkPanel *);
    const u8 *messageScript;
    const u8 *selectionScript;
    u8 unknownF4C[4];
    s32 role, number;
    u8 state, message, oldNumber, messageDelay, connection;
    u8 unknownF5D[3];
    s32 firstWindow, secondWindow, mainWindow;
    u8 redraw, unknownF6D;
    u16 timer;
    u32 completionScript, exitScript;
};
void LinkPanel_UpdateAnimations(struct LinkPanel *panel);
void LinkPanel_UpdateLastEntry(struct LinkPanel *panel);
void LinkPanel_InitMessageDisplay(struct LinkPanel *unused);
s32 LinkPanel_GetConnectionStatus(struct LinkPanel *panel);
void LinkPanel_UpdateStatusMessage(struct LinkPanel *panel);
void LinkPanel_Close(struct LinkPanel *panel);
void LinkPanel_RunCompletionScript(struct LinkPanel *panel);
void LinkPanel_UpdateExitTransition(struct LinkPanel *panel);
void LinkPanel_UpdateConnection(struct LinkPanel *panel);
s32 LinkPanel_Update(struct LinkPanel *panel);
s32 LinkPanel_Destroy(void);
s32 LinkPanel_InitFromScript(struct LinkPanel *panel);
struct LinkPanel *LinkPanel_Create(void);
#endif
