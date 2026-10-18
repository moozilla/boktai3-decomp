/* Tentative family and field names, based on static accesses. */
#define ActorResource_GetFixedId sub_081F2CD4
#include "global.h"
struct ActorResource {
    u8 unknown0[0xB3];
    u8 variant;
};
u32 sub_081F2CD4(struct ActorResource *p)
{
    switch (p->variant) {
        case 0:
            return 0x1F2;
        default:
            return 0x1F2;
    }
}
