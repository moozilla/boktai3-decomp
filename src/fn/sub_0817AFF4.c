#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.7%
// foundry draft (worker best 89.1%); what differs is mostly register allocation and instruction order
#define SceneVolume_CanActivate sub_0817AFF4
struct SceneVolumeOwner {
    u8 pad0[0x418];
    u8 disabled;
};
struct SceneVolume {
    u8 pad0[0x10C];
    struct SceneVolumeOwner *owner;
    u8 pad110[0x354];
    u32 flags;
    u8 pad468[11];
    u8 inhibited;
    u8 pad474[0x75];
    u8 mode;
    u8 pad4EA;
    u8 phase;
};
s32 sub_0817AFF4(struct SceneVolume *volume)
{
    if ((volume->mode == 2 || volume->mode == 10) && volume->phase == 4 &&
        (*(u32 *)((u8 *)volume + 0x464) & 7) && !volume->owner->disabled &&
        !*(u8 *)((u8 *)volume + 0x473))
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817AFF4);
#endif
