#include "scene_inventory_control.h"
#define SceneInventory_ConfirmConsumableAction sub_08062040
void SceneInventory_ConfirmConsumableAction(struct SceneInventoryControlView *s)
{
    switch (s->mode) {
        case 4:
        case 5:
            switch (sub_0806122C(s)) {
                case 0:
                    goto error;
                case 1:
                    Sound_PlaySong(0xDD);
                    sub_0806167C(s);
                    break;
                case -1:
                    Sound_PlaySong(0xDD);
                    sub_08061780(s);
                    break;
            }
            break;
        case 1:
            Sound_PlaySong(0xDD);
            sub_08061628(s);
            break;
        case 2:
            if (sub_0806122C(s) == 0) {
            error:
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0xDD);
                sub_08061914(s);
            }
            break;
    }
}
