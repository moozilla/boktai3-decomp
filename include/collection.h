#ifndef GUARD_COLLECTION_H
#define GUARD_COLLECTION_H
#include "scene_input.h"
#include "contact_swarm.h"
#include "display_sequence.h"
struct CollectionMenu {
    struct Task task;
    void *backgroundMap;
    u32 unknown1C, unknown20;
    struct SceneInputResource resource;
    struct SceneInputWidget background, cursor, completionBadge, pageIndicator;
    struct SceneInputWidget countDigits[4];
    u16 cursorColors[16];
    s16 pulseFrame;
    u16 unknown366;
    struct ContactSwarmGraphics graphics[27];
    struct ContactSwarmSprite entries[27];
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    u32 unknownB14[27];
    s16 page, unknownB82, column, row;
    u16 selectedEntry, unknownB8A;
    s16 exiting, fadeShift, fadeTimer;
    u16 unknownB92;
    s32 textWindow;
    const void *nameScript, *descriptionScript, *lockedDescriptionScript;
    u32 exitScript;
};
struct CollectionPalette {
    u32 header[5];
    u16 colors[256];
};

u32 Collection_GetPageEntryCount(u32 i);
void Collection_SetEntryUnlocked(s32 bit);
u32 Collection_IsEntryUnlocked(u32 i);
void Collection_UnlockEntryFromScript(void);
s32 Collection_CountUnlockedEntries();
u32 Collection_IsComplete(void);
s32 Collection_UpdateEntryVisibility(struct CollectionMenu *menu);
s32 Collection_UpdateCountDigits(struct CollectionMenu *p);
u32 Collection_UpdatePageIndicator(struct CollectionMenu *p);
s32 Collection_UpdateCursor(struct CollectionMenu *p);
void Collection_UpdateHighlightPulse(struct CollectionMenu *arg);
u32 Collection_UpdateSelectedEntryText(struct CollectionMenu *p);
void Collection_LoadPageBackground(struct CollectionMenu *p);
s32 Collection_RefreshPage(struct CollectionMenu *s);
u32 Collection_RefreshSelection(struct CollectionMenu *a);
extern const u32 gUnk_08613FB4[];
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_SetScript(const u8 *);
void Bg_SetPriority(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
#endif
