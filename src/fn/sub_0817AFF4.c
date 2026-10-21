// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
s32 SceneVolume_CanActivate(struct SceneVolume *volume)
{
    register u32 o asm("r1");
    register u8 *q asm("r0");
    if (!(volume->mode == 2 || volume->mode == 10))
        return 0;
    if (volume->phase != 4)
        return 0;
    o = 0x464;
    asm("" : "+r"(o));
    q = (u8 *)volume + o;
    if (!(*(u32 *)q & 7))
        return 0;
    if (volume->owner->disabled)
        return 0;
    o = 0x473;
    asm("" : "+r"(o));
    q = (u8 *)volume + o;
    if (*q)
        return 0;
    return 1;
}
