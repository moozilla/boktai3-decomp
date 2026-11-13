#ifndef GUARD_LINK_RECORD_PANEL_H
#define GUARD_LINK_RECORD_PANEL_H
#include "character_preview_actor.h"
#include "scene_input.h"
/* Two received records are a phase of the session's payload storage. */
struct LinkTimeRecord {
    u8 sourceName[16], displayName[16];
    u16 time, party;
    s32 times[6];
    u8 unknown3C[4];
};
struct LinkRecordPayload {
    u8 identity[0x20];
    u16 time, party;
    s32 scores[5];
    u8 unknown38[8];
    struct LinkTimeRecord peers[2];
};
struct LinkRecordIncomingStyle {
    u8 kind, variant, form, palette, extra;
    u8 unknown5[0x24 - 5];
};
struct LinkRecordPaletteCycle {
    u16 colors[16], frame;
    u8 unknown22[2];
};
struct LinkRecordPanel {
    struct Task task;
    void *backgroundResource, *paletteResource;
    struct SceneInputWidget widgets[7];
    struct SceneInputResource unknownResource, widgetResource;
    u16 entryValues[7], previousValues[7];
    void *widgetScript;
    u8 unknown320[4];
    struct LinkRecordPaletteCycle paletteCycle;
    struct CharacterPreviewActor previews[2];
    void (*update)(struct LinkRecordPanel *);
    u32 sourceScript;
    const u8 *contextScript;
    void *namesScript;
    u8 unknown13F8[4];
    u8 transition, selection, unknown13FE[2];
    s32 leftWindow, rightWindow, titleWindow;
    u8 entering, unknown140D;
    u16 timer, completionScript, exitScript;
};
void LinkRecordPanel_LoadBackground(struct LinkRecordPanel *s);
void LinkRecordPanel_DrawTime(struct LinkTimeRecord *p, u8 *text, u32 style, s32 index);
void LinkRecordPanel_DrawPeerTimes(struct LinkRecordPanel *p);
void LinkRecordPanel_CreateTextWindows(struct LinkRecordPanel *p);
void LinkRecordPanel_StartDescriptions(struct LinkRecordPanel *p);
void LinkRecordPanel_ClearTextWindows(struct LinkRecordPanel *p);
void LinkRecordPanel_InitializeEntryValues(struct LinkRecordPanel *p);
void LinkRecordPanel_InitializeWidgets(struct LinkRecordPanel *group);
void LinkRecordPanel_PositionWidgets(struct LinkRecordPanel *p);
void LinkRecordPanel_InitializePreviews(struct LinkRecordPanel *panel);
void LinkRecordPanel_HideDisplay(struct LinkRecordPanel *p);
void LinkRecordPanel_RemoveWidgets(struct LinkRecordPanel *p);
void LinkRecordPanel_ShowDialogue(void);
void LinkRecordPanel_OnIdle(void);
void LinkRecordPanel_RunExitScript(struct LinkRecordPanel *self);
void LinkRecordPanel_RunCompletionScript(struct LinkRecordPanel *self);
void LinkRecordPanel_UpdateExit(struct LinkRecordPanel *p);
void LinkRecordPanel_UpdateDelay(struct LinkRecordPanel *s);
s32 LinkRecordPanel_Update(struct LinkRecordPanel *p);
s32 LinkRecordPanel_Destroy(struct LinkRecordPanel *p);
s32 LinkRecordPanel_InitializeFromScript(struct LinkRecordPanel *s);
struct LinkRecordPanel *LinkRecordPanel_Create(void);
#endif
