/* Tentative family and field names, based on static accesses. */
#include "global.h"
struct ActorResource {
    u8 unknown0[0xB3];
    u8 variant;
};
u32 ActorResource_GetFixedId(struct ActorResource *p)
{
    switch (p->variant) {
        case 0:
            return 0x1F2;
        default:
            return 0x1F2;
    }
}
