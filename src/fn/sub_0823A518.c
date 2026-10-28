#include "global.h"
#include "area_effect_link.h"
struct L {
    u8 pad[0x5c];
    u8 arr[1];
};
u8 sub_0823A518(u8 *p)
{
    s32 index = *(s32 *)(p + 0x18);
    if (gLinkSession == NULL || index < 0)
        return 0;
    return ((struct L *)gLinkSession)->arr[index];
}