#include "scene_inventory_catalog.h"
extern const s8 gUnk_0824DF6C[];
void SceneInventory_BuildUnlockedItemCatalog(struct SceneInventoryScriptCatalog *p)
{
    s32 page;
    s32 i;
    p->page = 0;
    p->pageCount = 2;
    for (page = 0; page < p->pageCount; page++) {
        for (i = 0; i < 8; i++) {
            s32 kind = gUnk_0824DF6C[page * 8 + i];
            u32 enabled;
            ((u8 *)p->catalog)[page * 8 + i] = 0xFF;
            if (kind >= 0) {
                if (kind < 32)
                    enabled = gGameState->unlockedItems[0] & (1 << kind);
                else
                    enabled = gGameState->unlockedItems[1] & (1 << (kind - 32));
                if (enabled)
                    p->catalog[page * 8 + i] = kind;
            }
        }
    }
    for (; page <= 6; page++) {
        for (i = 0; i < 8; i++)
            p->catalog[page * 8 + i] = -1;
    }
    if (Script_SeekToKeyword(0x69))
        p->itemText = Script_GetTextSourcePc();
}
