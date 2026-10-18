#include "global.h"

extern u32 gUnk_030053F4, gUnk_030053F0;
extern u32 gUnk_02000560;
void Nidhoggr_CheckActivationTriggers(void *);
void Nidhoggr_UpdateState(void *);
s32 Collider_Refresh(void *);
void Camera_RequestShake(u32);
void sub_08194D70(void *);
void sub_0819465C(void *);
void Nidhoggr_UpdateBackgroundScroll(void *);
void sub_0819321C(void *);
void sub_0818ECFC(void *);
void Nidhoggr_ApproachCameraTarget(void *);
void sub_08225F88(void *);
void sub_082261B8(s32, s32);

static inline u32 Tst2(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}

static inline u32 Tst32(u32 mask, u32 *p)
{
    return *p & mask;
}

s32 Nidhoggr_Update(u8 *t)
{
    Nidhoggr_CheckActivationTriggers(t);
    Nidhoggr_UpdateState(t);
    if (Tst2(0x400, &gUnk_030053F4, &gUnk_030053F0) || Tst32(4, &gUnk_02000560))
        t[0x724c] = 0;
    if (t[0x724c]) {
        Collider_Refresh(t + 0x5dfc);
        Camera_RequestShake(0x80);
        t[0x724c]--;
    }
    sub_08194D70(t);
    sub_0819465C(t);
    Nidhoggr_UpdateBackgroundScroll(t);
    sub_0819321C(t);
    sub_0818ECFC(t + 0x54);
    Nidhoggr_ApproachCameraTarget(t);
    if (t[0x72a6])
        sub_08225F88(t + 0x7290);
    else
        sub_082261B8(*(s16 *)(t + 0x7290), *(s16 *)(t + 0x7292));
    (*(u16 *)(t + 0x7244))++;
    return 0;
}
