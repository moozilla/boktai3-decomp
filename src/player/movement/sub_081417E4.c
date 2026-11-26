#include "global.h"

void Camera_FollowPlayerWithoutCompletion(s32);

void Player_RestoreCameraFollow(u8 *p)
{
    Camera_FollowPlayerWithoutCompletion(8);
    *(u16 *)(p + 0x4B6) = 0;
}
