#include "global.h"
#define gGameState gUnk_02000710
/* Tentative inventory display family, inferred from slot and item consumers. */
#define InventoryPanel_SetEntryRow sub_08166E14
#define InventoryPanel_CreateEntry sub_08166D74
#include "game_state.h"
#define InventoryPanel_UpdateSelectedUnlock sub_08166ED4
struct InventoryPanelElement {
    u32 flags;
    u8 unknown4[0x5C];
};
struct InventoryPanel {
    u8 unknown0[0x2EC8];
    struct InventoryPanelElement selected[3];
};
s32 sub_08166D74(s32 *, s32, s32, s32, s32);
void sub_08166E14(struct InventoryPanel *, s32, s32, s32, s32, s32);
void sub_081654DC(s32, s32, s32, s32);
static inline void HideSelected(struct InventoryPanel *p, s32 x, s32 y)
{
    u32 mask = 1;
    struct InventoryPanelElement *e = p->selected;
    s32 i = 2;
    do {
        e->flags |= mask;
        e++;
    } while (--i >= 0);
    sub_081654DC(3, x, y, 0);
}
s32 sub_08166ED4(struct InventoryPanel *p, s32 x, s32 y, s32 query, s32 position)
{
    s32 value;
    s32 kind = sub_08166D74(&value, position, (s16)gUnk_02000710->unk50, 1, 0);
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
            sub_08166E14(p, x, y, value, query, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = value;
done:
    return kind;
}
