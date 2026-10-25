#include "global.h"

struct ContactPosition;
void sub_08085DD0(void);
void SceneActorReaction_RestoreAfterDelay(void);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

#ifdef NONMATCHING
// fuzzy: 31.8%
// scope shape 107 (differences once low registers are set aside)
// confidence=low: ghoul per-frame route step; consumes the arrival flag at 0x2B1, retargets on arrival, and calls the state callback when the route step did not move.
void sub_0808A664(u8 *p)
{
    u8 *owner;
    u8 *sub;
    u8 *route;
    u32 moved;
    s32 result;

    owner = *(u8 **)(p + 0x3D0);
    moved = 0;
    if (*(u8 *)(p + 0x2B1)) {
        *(u8 *)(p + 0x2B1) = 0;
        moved = 1;
    }
    if (moved) {
        *(u16 *)(p + 0x2D4) |= 0x100;
        *(u8 *)(p + 0x2B2) = 1;
        *(u8 *)(p + 0x2B0) = 0;
        *(u8 *)(p + 0x2B5) = 0x23;
        *(void **)(p + 0x21C) = sub_08085DD0;
        *(u16 *)(p + 0x2C8) = 0;
        *(u32 *)(p + 0x114) &= ~1;
        *(u32 *)(p + 0x104) = 0;
        *(u16 *)(owner + 0x63C) = *(u16 *)(owner + 0x63E);
    }
    sub = p + 0x3D4;
    if (*(u8 *)(p + 0x2B0)) {
        {
            s32 m = ~0x100;
            *(u16 *)(p + 0x2D4) = *(u16 *)(p + 0x2D4) & m;
        }
        *(u32 *)(p + 0x2D0) &= ~4;
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        *(u32 *)(sub + 0x98) = *(u16 *)(owner + 0x64A);
        *(u32 *)(sub + 0x9C) = *(u32 *)(sub + 0x48);
        sub[0x94] = 0;
        sub[0x95] = 0;
        sub[0x96] = 12;
        *(u8 *)(p + 0x2B2) = 1;
        *(u8 *)(p + 0x2B0) = 0;
        *(u8 *)(p + 0x2B5) = 14;
        *(void **)(p + 0x21C) = SceneActorReaction_RestoreAfterDelay;
        *(u16 *)(p + 0x2C8) = 0;
        *(u32 *)(p + 0x114) &= ~1;
        *(u32 *)(p + 0x104) = 0;
        *(u8 *)(p + 0x2A8) = 3;
        *(u8 *)(p + 0x2A9) = 0;
        *(u8 *)(p + 0x2AA) = 0;
        *(u32 *)(p + 0x2AC) = 0;
        *(u8 *)(p + 0x2B1) = 1;
        route = *(u8 **)(sub + 0x1FC);
        if (!Nav_IsPositionInMesh(route, (struct ContactPosition *)(p + 0x5C),
                                  (struct ContactPosition *)(owner + 0x62C))) {
            {
                s32 m = ~0x200;
                *(u16 *)(p + 0x2D4) = *(u16 *)(p + 0x2D4) & m;
            }
            result = 0;
        } else {
            result = Terrain_ApplyBlocked(route, (struct ContactPosition *)(p + 0x5C),
                                          (struct ContactPosition *)(owner + 0x62C));
            if (result) {
                if (result != 2) {
                    Memory_Clear(p + 0x64, 8);
                }
                result = 1;
            } else {
                {
                    s32 m = ~0x200;
                    *(u16 *)(p + 0x2D4) = *(u16 *)(p + 0x2D4) & m;
                }
                result = 0;
            }
        }
        if (!result) {
            Callback_InvokeActor(p, *(void **)(sub + 0x224));
        }
    } else {
        *(u32 *)(p + 0x2AC) += 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0808A664);
#endif
