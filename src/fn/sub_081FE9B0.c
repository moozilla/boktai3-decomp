#include "global.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
struct Orb {
    u8 pad[0x50];
    s16 v;
    u8 pad2[0x3ac - 0x52];
};
s32 sub_081FE9B0(s32 index)
{
    s32 result;
    u8 *m;
    if ((m = gSlotController) != NULL) {
        s32 off = index * 0x3ac;
        result = *(s16 *)(m + off + 0x50);
    } else {
        SunOrb_GetOrCreateManager();
        result = -1;
    }
    return result;
}