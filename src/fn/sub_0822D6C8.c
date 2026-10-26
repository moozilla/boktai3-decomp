#include "global.h"
struct E {
    u8 b[16];
};
struct G {
    u8 pad[0x1c0];
    struct E e[1];
};
extern struct G *gGameState;
struct E *Inventory_GetEquipmentEntry(u32 i)
{
    return &gGameState->e[i];
}
