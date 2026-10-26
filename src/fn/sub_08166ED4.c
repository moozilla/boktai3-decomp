#include "global.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
#include "game_state.h"
struct InventoryPanelElement {
    u32 flags;
    u8 unknown4[0x5C];
};
struct InventoryPanel {
    u8 unknown0[0x2EC8];
    struct InventoryPanelElement selected[3];
};
s32 InventoryPanel_CreateEntry(s32 *, s32, s32, s32, s32);
void InventoryPanel_SetEntryRow(struct InventoryPanel *, s32, s32, s32, s32, s32);
void SceneHud_FillDigitCells(s32, s32, s32, s32);
static inline void HideSelected(struct InventoryPanel *p, s32 x, s32 y)
{
    u32 mask = 1;
    struct InventoryPanelElement *e = p->selected;
    s32 i = 2;
    do {
        e->flags |= mask;
        e++;
    } while (--i >= 0);
    SceneHud_FillDigitCells(3, x, y, 0);
}
s32 InventoryPanel_UpdateSelectedUnlock(struct InventoryPanel *p, s32 x, s32 y, s32 query,
                                        s32 position)
{
    s32 value;
    s32 kind = InventoryPanel_CreateEntry(&value, position, (s16)gGameState->unk50, 1, 0);
    if (query) {
        if (kind != 4) {
            kind = -1;
            goto done;
        }
    } else {
        if (kind != 4) {
            HideSelected(p, x, y);
            return -1;
        }
        if (value > 0)
            InventoryPanel_SetEntryRow(p, x, y, value, query, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = value;
done:
    return kind;
}
