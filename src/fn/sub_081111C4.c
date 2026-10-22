#include "global.h"
struct S {
    u8 pad[0x54];
    s32 x54;
};
extern struct S *gUnk_020004B8;
u8 SceneTilemap_IsActive(void);

s32 sub_081111C4(void)
{
    if (!SceneTilemap_IsActive())
        return -1;
    else
        return gUnk_020004B8->x54 - 12;
}
