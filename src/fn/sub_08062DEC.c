#include "scene_inventory_catalog.h"
extern const s8 gUnk_0824DF3C[];
void SceneInventory_BuildUnlockedEquipmentCatalog(struct SceneInventoryScriptCatalog *p)
{
    s32 page;
    s32 i;
    p->page = 0;
    p->pageCount = 6;
    for (page = 0; page < p->pageCount; page++) {
        for (i = 0; i < 8; i++) {
            s32 kind = gUnk_0824DF3C[page * 8 + i];
            u32 enabled;
            ((u8 *)p->catalog)[page * 8 + i] = 0xFF;
            if (kind >= 0) {
                if (kind < 32)
                    enabled = gGameState->unlockedEquipment[0] & (1 << kind);
                else
                    enabled = gGameState->unlockedEquipment[1] & (1 << (kind - 32));
                if (enabled)
                    p->catalog[page * 8 + i] = kind;
            }
        }
    }
    for (; page <= 6; page++) {
        u32 mask = 0xFF;
        for (i = 0; i < 8; i++) {
            u32 value = ((u8 *)p->catalog)[page * 8 + i];
            p->catalog[page * 8 + i] = value | mask;
        }
    }
    if (Script_SeekToKeyword(0x77))
        p->equipmentText = Script_GetTextSourcePc();
    if (Script_SeekToKeyword(0x63)) {
        p->equipmentLabel = Script_GetTextSourcePc();
        if (p->equipmentLabel)
            p->equipmentLabel =
                (u32)Text_LookupString(Script_ParseStringRef((void *)p->equipmentLabel) + 1);
    }
}
