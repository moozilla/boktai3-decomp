#include "global.h"
#include "ui/widgets/scene_input_grid.h"
#include "system/game_state/game_state.h"
struct StorageCategoryMenu {
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
void Storage_UpdateBrowseIntro(struct StorageCategoryMenu *);
void Storage_SetState(struct StorageCategoryMenu *,
                             void (*)(struct StorageCategoryMenu *), u32);
void Storage_CompleteCategoryRemoval(struct StorageCategoryMenu *p)
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
        Storage_SetState(p, Storage_UpdateBrowseIntro, 0);
    }
}
