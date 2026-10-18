#include "global.h"
struct S {
    u8 p[0x18];
    s16 id;
};
extern struct S *gUnk_020004B4;
void Camera_FollowPlayerWithoutCompletion(u32);
static inline s32 gid(void)
{
    if (gUnk_020004B4 == 0)
        return -1;
    return gUnk_020004B4->id;
}
void Player_ClearMovementLock(u8 *s)
{
    if (*(s32 *)(s + 0x18) == gid())
        Camera_FollowPlayerWithoutCompletion(8);
    *(u16 *)(s + 0x4b6) = 0;
}
