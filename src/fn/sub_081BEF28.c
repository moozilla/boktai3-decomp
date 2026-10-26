#include "global.h"
#include "scene_input_grid.h"
#include "game_state.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x1558];
    struct SceneInputPair transition;
    u8 unknown164C[0x1754 - 0x164C];
    u8 category;
    u8 unknown1755[3];
    u8 categoryKinds[0xE];
    u8 selection;
    u8 unknown1767[0x1790 - 0x1767];
    u8 slots[1];
};
void Inventory_ClearItemSlot(u32);
void Inventory_RemoveEquipmentEntry(s32);
void Sound_PlaySong(u32);
void SceneInventory_UpdateBrowseIntro(struct SceneInventoryCategoryMenu *);
void SceneInventory_SetState(struct SceneInventoryCategoryMenu *,
                             void (*)(struct SceneInventoryCategoryMenu *), u32);
void SceneInventory_CompleteCategoryRemoval(struct SceneInventoryCategoryMenu *p)
{
    if (SceneInput_TickPairTransition(&p->transition)) {
        u8 *kinds = p->categoryKinds;
        switch (kinds[p->category] >> 1) {
            case 0:
                Inventory_ClearItemSlot(p->slots[p->selection]);
                break;
            case 1:
                Inventory_RemoveEquipmentEntry(p->slots[p->selection]);
                break;
            case 2: {
                u32 address = p->slots[p->selection];
                struct GameState *state = gGameState;
                struct GameState *indexed;
                s16 *slot;
                address <<= 1;
                address += (u32)state;
                indexed = (struct GameState *)address;
                slot = &indexed->table160[0];
                *slot = -1;
            } break;
        }
        Sound_PlaySong(0x10E);
        SceneInventory_SetState(p, SceneInventory_UpdateBrowseIntro, 0);
    }
}
