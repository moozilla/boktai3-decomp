#include "global.h"

struct HudSlot {
    u32 flags;
    u8 pad4[0x5C];
};

struct ActionMenu {
    u8 pad0[0x1428];
    struct HudSlot slots[(0x41E8 - 0x1428) / 0x60];
    u32 flags41E8;
    u8 pad41EC[0x4200 - 0x41EC];
    u8 cursor0[0x48];
    u32 flags4248;
    u8 pad424C[0x4260 - 0x424C];
    u8 cursor1[0x4387 - 0x4260];
    u8 entry1;
    u8 pad4388[2];
    u8 entry0;
    u8 pad438B[0x4D9C - 0x438B];
    s16 index0;
    s16 index1;
};

s32 MenuDisplay_FindEntryIndex(struct ActionMenu *p, s32 entry, s32 group);
void SceneHud_SetActionCursorPosition(void *, s32);
void SceneHud_GetActionIconPosition(void *, s32);

void sub_0816676C(struct ActionMenu *p, s32 mode)
{
    if (mode == 0) {
        if ((p->index0 = MenuDisplay_FindEntryIndex(p, p->entry0, 0)) >= 0) {
            p->slots[p->index0 + 0x1E].flags |= 1;
            SceneHud_SetActionCursorPosition(p->cursor0, p->entry0);
        }
        if ((p->index1 = MenuDisplay_FindEntryIndex(p, p->entry1, 0)) >= 0) {
            p->slots[p->index1 + 0x1E].flags |= 1;
            SceneHud_SetActionCursorPosition(p->cursor1, p->entry1);
        }
    } else if (mode == 3) {
        if ((p->index0 = MenuDisplay_FindEntryIndex(p, p->entry0, 3)) >= 0) {
            p->slots[p->index0 + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(p->cursor0, p->entry0);
        }
        if ((p->index1 = MenuDisplay_FindEntryIndex(p, p->entry1, 3)) >= 0) {
            p->slots[p->index1 + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(p->cursor1, p->entry1);
        }
    } else if (mode == 2) {
        if ((p->index0 = MenuDisplay_FindEntryIndex(p, p->entry0, 2)) >= 0) {
            p->slots[p->index0 + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(p->cursor0, p->entry0);
        }
        if ((p->index1 = MenuDisplay_FindEntryIndex(p, p->entry1, 2)) >= 0) {
            p->slots[p->index1 + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(p->cursor1, p->entry1);
        }
    } else if (mode == 1) {
        if ((p->index0 = MenuDisplay_FindEntryIndex(p, p->entry0 - 8, 1)) >= 0) {
            p->slots[p->index0 + 0x26].flags |= mode;
            SceneHud_SetActionCursorPosition(p->cursor0, p->entry0);
        }
        if ((p->index1 = MenuDisplay_FindEntryIndex(p, p->entry1 - 8, 1)) >= 0) {
            p->slots[p->index1 + 0x26].flags |= mode;
            SceneHud_SetActionCursorPosition(p->cursor1, p->entry1);
        }
    }
    if (p->index0 >= 0)
        p->flags41E8 &= ~1;
    if (p->index1 >= 0)
        p->flags4248 &= ~1;
}
