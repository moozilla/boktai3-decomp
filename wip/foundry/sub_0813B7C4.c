#define SceneActor_UpdateCollisionMask sub_0813B7C4
#include "global.h"
struct SceneActorCollisionMask {
    u8 unknown0[0x338];
    u32 flags338;
    u8 unknown33C[0x418-0x33C];
    u8 mode418;
    u8 unknown419[0x534-0x419];
    u16 kind534;
    u16 unknown536;
    u16 active538;
};
extern u8 *gUnk_02000710;
void sub_0821FF7C(void *, s32, s32, s32);
static inline u8 IsActive(struct SceneActorCollisionMask *p)
{
    u8 result;
    if (!p) return 0;
    result = 0;
    if (p->active538 && p->kind534 == 8) result = 1;
    return result;
}
void sub_0813B7C4(struct SceneActorCollisionMask *p)
{
    s32 zero, mask, mode;
    if (p->mode418 == 6) {
        s32 unused = *(s16 *)(gUnk_02000710 + 0x40) + 10;
        zero = 0;
        mask = p->flags338 | 2;
        mode = 1;
    } else if (p->mode418 == 0) {
        zero = 0;
        mask = p->flags338 | 1;
        mode = 2;
    } else {
        zero = 0;
        mask = p->flags338 | 2;
        mode = 1;
    }
    if (IsActive(p) || p->flags338 == 0x40) mask = 0x40;
    sub_0821FF7C((u8 *)p + 0x230, zero, mask, mode);
}
