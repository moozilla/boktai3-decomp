#include "ui/town/shop_control.h"
s32 Shop_ClassifySelectedAction(struct SceneInventoryControlView *panel)
{
    switch (panel->category) {
        case 0:
            return Shop_ClassifyEquipmentAction(panel);
        case 1:
            return Shop_ClassifyItemAction(panel);
        case 2:
            return Shop_ClassifyConsumableAction(panel);
    }
}
