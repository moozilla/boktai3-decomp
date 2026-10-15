#include "scene_inventory_control.h"
void SceneInventory_ConfirmConsumableAction(struct SceneInventoryControlView *s)
{
    switch (s->mode) {
        case 4:
        case 5:
            switch (SceneInventory_ClassifyConsumableAction(s)) {
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
            if (SceneInventory_ClassifyConsumableAction(s) == 0) {
            error:
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0xDD);
                SceneInventory_BeginRemovalConfirmation(s);
            }
            break;
    }
}
