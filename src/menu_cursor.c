#include "menu_cursor.h"

static inline void MenuCursor_ClearRequests(struct MenuCursor *cursor)
{
    cursor->stateFlags = 0;
}

static inline void ClearCursorVector(void *dest, u32 *zero, u32 control)
{
    *zero = 0;
    CpuSet(zero, dest, control);
}

/* Keep the owner-relative index calculation before adding the entry-table field. */
static inline struct MenuCursorEntry *EntryForIndex(struct MenuCursor *cursor, s32 index)
{
    u32 tableOffset = (u32) & ((struct MenuCursor *)0)->entries;
    return (void *)(index * sizeof(struct MenuCursorEntry) + (u32)cursor + tableOffset);
}

void MenuCursor_SetUpdate(struct MenuCursor *cursor, void (*update)(struct MenuCursor *))
{
    cursor->callback = update;
    cursor->timer = 0;
}

void MenuCursor_SetPositionFromSelection(struct MenuCursor *cursor)
{
    if (cursor->selectedIndex >= 0) {
        cursor->position.x = cursor->entries[cursor->selectedIndex].x;
        cursor->position.y = cursor->entries[cursor->selectedIndex].y - 10;
    }
}

s32 MenuCursor_SelectNearestEnabledEntry(struct MenuCursor *cursor)
{
    s32 x = cursor->sprite.position.x;
    s32 y = cursor->sprite.position.y;
    s32 nearest = 0;
    s32 dx = x - cursor->entries[0].x;
    s32 dy = y - cursor->entries[0].y;
    s32 distance = dx * dx + dy * dy;
    s32 i;
    for (i = 1; i < cursor->entryCount; i++) {
        u16 one = 1;
        if (!(cursor->entries[i].bounds.flags & one)) {
            s32 candidate;
            dx = x - cursor->entries[i].x;
            dy = y - cursor->entries[i].y;
            candidate = dx * dx + dy * dy;
            if (candidate < distance) {
                distance = candidate;
                nearest = i;
            }
        }
    }
    return nearest;
}

s32 MenuCursor_FindEntryAtPosition(struct MenuCursor *cursor)
{
    s16 *position = &cursor->sprite.position.x;
    s32 i = 0;
    if (i < cursor->entryCount) {
        u32 mask = 1;
        s32 count = cursor->entryCount;
        struct MenuCursorBounds *entry = &cursor->entries[0].bounds;
        do {
            if (!(entry->flags & mask) && position[0] >= entry->left &&
                position[0] <= entry->right && position[1] >= entry->top &&
                position[1] <= entry->bottom)
                return i;
            entry = (void *)((u8 *)entry + sizeof(struct MenuCursorEntry));
            ++i;
        } while (i < count);
    }
    return -1;
}

s32 MenuCursor_ProcessInput(struct MenuCursor *cursor)
{
    s16 *position = &cursor->position.x;
    s32 step;
    s32 old, entry;
    step = (gPadState[0] & 2) ? 2 : 1;
    if (gPadState[0] & 0x20)
        position[0] -= step;
    else if (gPadState[0] & 0x10)
        position[0] += step;
    if (gPadState[0] & 0x40)
        position[1] -= step;
    else if (gPadState[0] & 0x80)
        position[1] += step;
    if (position[0] < 8)
        position[0] = 8;
    else if (position[0] > 232)
        position[0] = 232;
    if (position[1] < 8)
        position[1] = 8;
    else if (position[1] > 160)
        position[1] = 160;
    if (gPadState[1] & 0x200) {
        Sound_PlaySong(0xDC);
        if (cursor->selectedIndex >= 0) {
            old = cursor->selectedIndex;
            entry = cursor->entries[old].bounds.previous;
            goto validate;
        }
        goto nearest;
    } else if (gPadState[1] & 0x100) {
        Sound_PlaySong(0xDC);
        if (cursor->selectedIndex >= 0) {
            old = cursor->selectedIndex;
            entry = cursor->entries[old].bounds.next;
            goto validate;
        }
        goto nearest;
    }
    goto noInput;
validate: {
    struct MenuCursorEntry *point = EntryForIndex(cursor, entry);
    u16 one = 1;
    if (point->bounds.flags & one)
        entry = old;
}
    goto apply;
nearest:
    entry = MenuCursor_SelectNearestEnabledEntry(cursor);
apply:
    cursor->selectedIndex = entry;
    MenuCursor_SetPositionFromSelection(cursor);
    MenuCursor_SetUpdate(cursor, MenuCursor_TickShake);
    return 1;
noInput:
    return 0;
}

void MenuCursor_TickShake(struct MenuCursor *cursor)
{
    s16 *position = &cursor->sprite.position.x;
    s32 value = gUnk_086149C4[((cursor->timer << 7) + 0x40) & 0xFF] * 4;
    s32 offset;
    if (value >= 0)
        offset = value >> 13;
    else
        offset = -((-value) >> 13);
    position[0] = cursor->position.x + offset;
    position[1] = cursor->position.y;
    if (++cursor->timer > 2)
        MenuCursor_SetUpdate(cursor, MenuCursor_UpdateInput);
}

void MenuCursor_OnSelectionFlag(struct MenuCursor *cursor)
{
    cursor->stateFlags |= 2;
}

void MenuCursor_OnHidden(struct MenuCursor *cursor)
{
    u32 m = 1;
    cursor->stateFlags |= m;
}

void MenuCursor_UpdateInput(struct MenuCursor *cursor)
{
    u32 m;
    cursor->selectedIndex = MenuCursor_FindEntryAtPosition(cursor);
    cursor->sprite.position = cursor->position;
    if (MenuCursor_ProcessInput(cursor) == 0) {
        m = 2;
        cursor->stateFlags |= m;
    }
}

void MenuCursor_UpdateState(struct MenuCursor *cursor)
{
    u32 *state;
    MenuCursor_ClearRequests(cursor);
    if (cursor->callback)
        Callback_InvokeActor(cursor, cursor->callback);
    {
        u32 one = 1;
        u32 requested = cursor->stateFlags & one;
        state = &cursor->stateFlags;
        if (requested)
            cursor->sprite.flags |= one;
        else
            cursor->sprite.flags &= ~1;
    }
    if (cursor->selectedIndex >= 0) {
        struct MenuCursorEntry *entry = EntryForIndex(cursor, cursor->selectedIndex);
        u32 mask = 2;
        if (entry->bounds.flags & mask) {
            u32 clear = ~2;
            *state = (*state & clear) | 4;
        }
    } else {
        u32 clear = ~2;
        *state &= clear;
    }
}

void MenuCursor_OnDestroy(struct MenuCursor *cursor)
{
    Anim_Remove(&cursor->sprite);
}

void MenuCursor_CopyEntries(struct MenuCursor *cursor, u32 count, struct MenuCursorEntry *entries)
{
    s32 i;
    struct MenuCursorEntry *dest;
    if (count <= 16) {
        cursor->entryCount = count;
        i = 0;
        for (; i < cursor->entryCount; i++) {
            cursor->entries[i] = entries[i];
        }
    }
}

void MenuCursor_Initialize(struct MenuCursor *cursor, u32 palette)
{
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0xA2DB);
    if (resource) {
        cursor->resource = *resource;
        ResourceHeader_Relocate(&cursor->resource, resource);
        Widget_InitLinked(&cursor->sprite, &cursor->resource, 2, 0x30, (u8)palette, 0, 0x3C, 0);
        cursor->entryCount = 0;
        cursor->selectedIndex = 0xFFFF;
    }
}

void MenuCursor_Destroy(struct MenuCursor *task)
{
    if (task != NULL)
        Task_Kill(task);
}

void MenuCursor_SetSelection(struct MenuCursor *menu, u32 selection)
{
    if (menu != NULL) {
        menu->selectedIndex = selection;
        MenuCursor_SetPositionFromSelection(menu);
    }
}

void MenuCursor_Hide(struct MenuCursor *cursor)
{
    if (cursor != 0)
        MenuCursor_SetUpdate(cursor, MenuCursor_OnHidden);
}

void MenuCursor_EnableInput(struct MenuCursor *cursor)
{
    if (cursor != 0)
        MenuCursor_SetUpdate(cursor, MenuCursor_UpdateInput);
}

void MenuCursor_SetSelectionFlag(struct MenuCursor *cursor)
{
    if (cursor != 0)
        MenuCursor_SetUpdate(cursor, MenuCursor_OnSelectionFlag);
}

struct MenuCursorEntry *MenuCursor_GetEntry(struct MenuCursor *cursor, s32 index)
{
    if (!cursor)
        return 0;
    if (index < 0 || index >= cursor->entryCount)
        return 0;
    return &cursor->entries[index];
}

struct MenuCursor *MenuCursor_Create(u32 palette, s32 selection, s32 count,
                                     struct MenuCursorEntry *entries)
{
    struct MenuCursor *cursor = Task_Create(8, sizeof(*cursor));
    u32 zero;

    if (cursor == NULL)
        return NULL;
    Task_SetCallbacks(cursor, (void (*)(void *))MenuCursor_UpdateState,
                      (void (*)(void *))MenuCursor_OnDestroy);
    MenuCursor_Initialize(cursor, palette);
    MenuCursor_CopyEntries(cursor, count, entries);
    ClearCursorVector(&cursor->sprite.position, &zero, 0x05000002);
    ClearCursorVector(&cursor->position, &zero, 0x05000002);
    if (selection >= 0 && selection < count)
        cursor->selectedIndex = selection;
    else
        cursor->selectedIndex = 0xFFFF;
    MenuCursor_SetPositionFromSelection(cursor);
    MenuCursor_SetUpdate(cursor, MenuCursor_UpdateInput);
    return cursor;
}
