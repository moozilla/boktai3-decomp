#ifndef GUARD_SAVE_MENU_H
#define GUARD_SAVE_MENU_H
#include "engine.h"
#include "scene_input_widget.h"
#include "character_preview_actor.h"
struct SaveMenuPaletteCycle {
    u16 colors[16], frame;
    u8 unknown22[2];
};
struct SaveMenu {
    struct Task task;
    void *backgroundResource, *alternateBackgroundResource, *paletteResource;
    struct SceneInputWidget widgets[18];
    struct SceneInputResource auxiliaryResource, widgetResource;
    u16 glyphs[18], cachedGlyphs[18];
    void *widgetScript;
    struct SaveMenuPaletteCycle paletteCycle;
    struct CharacterPreviewActor preview;
    void (*update)(struct SaveMenu *);
    const u8 *titleScript, *scoreScript;
    void *nameScript;
    const u8 *statusScript;
    u32 flags;
    s32 localPlayer;
    u8 state, unknown1001[7];
    s32 windows[7];
    u8 entering, unknown1025;
    u16 timer, unknown1028, exitScript, unknown102C, skip, remaining, step;
};
void SaveMenu_UpdateSaveDelay(struct SaveMenu *menu);
void SaveMenu_WaitForSaveTransition(struct SaveMenu *menu);
void SaveMenu_UpdateSlotCommit(struct SaveMenu *menu);
void SaveMenu_UpdateExitDelay(struct SaveMenu *menu);
void SaveMenu_UpdateSaveFailure(struct SaveMenu *menu);
void SaveMenu_UpdateScoreTransfer(struct SaveMenu *menu);
void SaveMenu_UpdateScoreReveal(struct SaveMenu *menu);
s32 SaveMenu_Update(struct SaveMenu *menu);
u32 SaveMenu_Destroy(struct SaveMenu *menu);
s32 SaveMenu_InitializeFromScript(struct SaveMenu *menu);
struct SaveMenu *SaveMenu_CreateFromScript(void);
#endif
