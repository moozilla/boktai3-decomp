#include "scene_inventory_control.h"
#define SceneInventory_ConfirmItemAction sub_08061FAC
void SceneInventory_ConfirmItemAction(struct SceneInventoryControlView *s)
{
    switch (s->mode) {
        case 4:
        case 5:
            switch (sub_080611AC(s)) {
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
            if (sub_080611AC(s) == 0) {
            error:
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0xDD);
                sub_08061914(s);
            }
            break;
    }
}
