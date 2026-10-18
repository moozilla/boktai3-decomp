#include "global.h"
#include "directional_interaction.h"
struct WoodenCrate {
    u8 unknown0[0xC];
    void *drawable;
    u8 unknown10[0x56];
    u16 displayFlags;
    u8 unknown68[0x3C];
    u16 timer;
    u8 unknownA6[0x24];
    s16 paletteVariant;
    u8 unknownCC;
    u8 flashTimer;
};
static inline void UpdateTimedFlags(struct WoodenCrate *crate)
{
    if (crate->timer) {
        crate->displayFlags |= 4;
        --crate->timer;
    } else {
        crate->displayFlags &= ~4;
    }
}
void WoodenCrate_UpdatePaletteTimers(struct WoodenCrate *crate)
{
    u8 *bytes = (u8 *)crate;
    UpdateTimedFlags(crate);
    if (bytes[0xCD]) {
        AnimData_SelectPaletteBank(crate->drawable, 0x192);
        --bytes[0xCD];
    } else {
        switch (*(s16 *)(bytes + 0xCA)) {
        case 1:
            AnimData_SelectPaletteBank(crate->drawable, 0x1A0);
            break;
        case 0:
            AnimData_SelectPaletteBank(crate->drawable, 0x1A1);
            break;
        }
    }
}
