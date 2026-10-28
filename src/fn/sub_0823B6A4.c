#include "global.h"
#include "area_effect_link.h"
struct S {
    u8 p[0x18];
    s16 id;
};
void Camera_FollowPlayerWithoutCompletion(u32);
static inline s32 gid(void)
{
    if (((struct S *)gLinkSession) == 0)
        return -1;
    return ((struct S *)gLinkSession)->id;
}
void Player_ClearMovementLock(u8 *s)
{
    if (*(s32 *)(s + 0x18) == gid())
        Camera_FollowPlayerWithoutCompletion(8);
    *(u16 *)(s + 0x4b6) = 0;
}
