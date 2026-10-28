#include "global.h"
#include "area_effect_link.h"

s32 Div(s32, s32);
void Camera_SnapToWithoutCompletion(void *);

struct Q {
    u8 pad0[0x2C];
    u8 id;
    u8 pad2D[3];
    s16 x, y, z;
    u8 pad36[0x4A8 - 0x36];
    s16 cx, cy, cz;
    u8 pad4AE[2];
    s16 tx, ty, tz;
};

static inline s32 ControllerId(void)
{
    if (!((u8 *)gLinkSession))
        return -1;
    return *(s16 *)(((u8 *)gLinkSession) + 0x18);
}
// confidence=low: smooths the camera anchor toward actor position and motion offset.
#define Player_SmoothCameraAnchor sub_0823B7A4
void Player_SmoothCameraAnchor(struct Q *p)
{
    s32 other;
    u32 id;
    {
        s32 value = p->x;
        s32 offset = p->tx;
        s32 sum = value + offset;
        s16 *target = &p->cx;
        s32 previous = *target;
        previous = previous * 4 + 0;
        sum += previous;
        *target = Div(sum, 5);
    }
    {
        s32 value = p->y;
        s32 offset = p->ty;
        s32 sum = value + offset;
        s16 *target = &p->cy;
        s32 previous = *target;
        previous = previous * 4 + 150;
        sum += previous;
        *target = Div(sum, 5);
    }
    {
        s32 value = p->z;
        s32 offset = p->tz;
        s32 sum = value + offset;
        s16 *target = &p->cz;
        s32 previous = *target;
        previous = previous * 4 + 0;
        sum += previous;
        *target = Div(sum, 5);
    }
    id = p->id;
    other = ControllerId();
    if (id == other) {
        Camera_SnapToWithoutCompletion(&p->cx);
    }
}