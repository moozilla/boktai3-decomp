#include "global.h"

#define MenuCursor_Create sub_081BB1A0

struct ResourceSpriteEntry {
    s32 values[4];
};
struct ResourceSpriteHeader {
    u32 words[8];
};
struct ResourceSprite {
    u8 pad0[0x18];
    u8 sprite[0x60];
    struct ResourceSpriteHeader resource;
    u32 position[2];
    s16 selection;
    s16 entryCount;
    struct ResourceSpriteEntry entries[16];
};

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_081BAFD4(void);
void MenuCursor_OnDestroy(void);
void ResourceSprite_Initialize(struct ResourceSprite *, u32);
void ResourceSprite_CopyEntries(struct ResourceSprite *, u32, struct ResourceSpriteEntry *);
void MenuCursor_SetPositionFromSelection(struct ResourceSprite *);
void SelectionCursor_SetState(void *, void (*)(void));
void MenuCursor_UpdateInput(void);

static inline void ClearCursorVector(void *dest, u32 *zero, u32 control)
{
    *zero = 0;
    CpuSet(zero, dest, control);
}

struct ResourceSprite *sub_081BB1A0(u32 palette, s32 selection, s32 count,
                                    struct ResourceSpriteEntry *entries)
{
    struct ResourceSprite *cursor = Task_Create(8, 0x1B0);
    u32 zero;

    if (cursor == NULL)
        return NULL;
    Task_SetCallbacks(cursor, sub_081BAFD4, MenuCursor_OnDestroy);
    ResourceSprite_Initialize(cursor, palette);
    ResourceSprite_CopyEntries(cursor, count, entries);
    ClearCursorVector(cursor->sprite + 0x20, &zero, 0x05000002);
    ClearCursorVector(cursor->position, &zero, 0x05000002);
    if (selection >= 0 && selection < count)
        cursor->selection = selection;
    else
        cursor->selection = 0xFFFF;
    MenuCursor_SetPositionFromSelection(cursor);
    SelectionCursor_SetState(cursor, MenuCursor_UpdateInput);
    return cursor;
}
