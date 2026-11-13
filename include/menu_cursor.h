#ifndef GUARD_MENU_CURSOR_H
#define GUARD_MENU_CURSOR_H
#include "scene_input.h"

struct MenuCursorEntry {
    s16 x, y;
    s16 left, top, right, bottom;
    u8 previous, next;
    u16 flags;
};
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
struct MenuCursorEntry *Registry3F74_FindFrameOffset(struct MenuCursor *, s32);
#endif
