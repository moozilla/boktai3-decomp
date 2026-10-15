#include "scene_inventory_control.h"
void SceneInventory_ConfirmEquipmentAction(struct SceneInventoryControlView *s)
{
    switch (s->mode) {
        case 4:
        case 5:
            switch (SceneInventory_ClassifyEquipmentAction(s)) {
                case 0:
                    goto error;
                case 1:
                    Sound_PlaySong(0xDD);
                    SceneInventory_BeginPurchaseConfirmation(s);
                    break;
                case -1:
                    Sound_PlaySong(0xDD);
                    SceneInventory_BeginSaleConfirmation(s);
                    break;
            }
            break;
        case 1:
            Sound_PlaySong(0xDD);
            SceneInventory_BeginReposition(s);
            break;
        case 2:
            if (SceneInventory_ClassifyEquipmentAction(s) == 0) {
            error:
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0xDD);
                SceneInventory_BeginRemovalConfirmation(s);
            }
            break;
    }
}
