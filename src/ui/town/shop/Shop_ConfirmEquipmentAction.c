#include "ui/town/shop_control.h"
void Shop_ConfirmEquipmentAction(struct ShopControlView *s)
{
    switch (s->mode) {
        case 4:
        case 5:
            switch (Shop_ClassifyEquipmentAction(s)) {
                case 0:
                    goto error;
                case 1:
                    Sound_PlaySong(0xDD);
                    Shop_BeginPurchaseConfirmation(s);
                    break;
                case -1:
                    Sound_PlaySong(0xDD);
                    Shop_BeginSaleConfirmation(s);
                    break;
            }
            break;
        case 1:
            Sound_PlaySong(0xDD);
            Shop_BeginReposition(s);
            break;
        case 2:
            if (Shop_ClassifyEquipmentAction(s) == 0) {
            error:
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0xDD);
                Shop_BeginRemovalConfirmation(s);
            }
            break;
    }
}
