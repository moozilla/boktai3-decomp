#ifndef GUARD_MENU_CURSOR_H
#define GUARD_MENU_CURSOR_H
#include "scene_input.h"

struct MenuCursorBounds {
    s16 left, top, right, bottom;
    u8 previous, next;
    u16 flags;
};
struct MenuCursorEntry {
    s16 x, y;
    struct MenuCursorBounds bounds;
} __attribute__((aligned(4)));
struct MenuCursor {
    struct Task task;
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    struct ContactPosition position;
    s16 selectedIndex, entryCount;
    struct MenuCursorEntry entries[16];
    u32 stateFlags;
    void (*callback)(struct MenuCursor *);
    s32 timer;
};
struct MenuCursor *MenuCursor_Create(u32 palette, s32 selection, s32 count,
                                     struct MenuCursorEntry *entries);
void MenuCursor_Hide(struct MenuCursor *cursor);
void MenuCursor_EnableInput(struct MenuCursor *cursor);
void MenuCursor_SetSelectionFlag(struct MenuCursor *cursor);
struct MenuCursorEntry *MenuCursor_GetEntry(struct MenuCursor *, s32);
void MenuCursor_SetUpdate(struct MenuCursor *, void (*)(struct MenuCursor *));
void MenuCursor_SetPositionFromSelection(struct MenuCursor *);
s32 MenuCursor_SelectNearestEnabledEntry(struct MenuCursor *);
s32 MenuCursor_FindEntryAtPosition(struct MenuCursor *);
s32 MenuCursor_ProcessInput(struct MenuCursor *);
void MenuCursor_TickShake(struct MenuCursor *);
void MenuCursor_OnSelectionFlag(struct MenuCursor *);
void MenuCursor_OnHidden(struct MenuCursor *);
void MenuCursor_UpdateInput(struct MenuCursor *);
void MenuCursor_UpdateState(struct MenuCursor *);
void MenuCursor_OnDestroy(struct MenuCursor *);
void MenuCursor_CopyEntries(struct MenuCursor *, u32, struct MenuCursorEntry *);
void MenuCursor_Initialize(struct MenuCursor *, u32);
void MenuCursor_Destroy(struct MenuCursor *);
void MenuCursor_SetSelection(struct MenuCursor *, u32);
#endif
