#include "global.h"
// MATCHING_CONCESSION: register-pinning
s32 Callback_InvokeActor(void *arg, void *function);
s32 Route_GetCurrentPointNodeId(void *);
u8 sub_0812F3CC(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
static inline void setf(u32 *a, u32 m)
{
    *a |= m;
}
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void sub_08130F60(u8 *s)
{
    u8 *p = s + 0x448;
    if (TakeFlag(s + 0x2B1)) {
        u32 target = *(u32 *)(s + 0x224);
        u32 two = 2;
        u32 zero;
        u32 *flags;
        s32 mask;
        s32 field;
        s32 off;
        {
            u8 *taken = s + 0x2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            field = 0x2B0;
            do {
                s[field] = zero;
            } while (0);
            field += 5;
            s[off = field] = two;
            field -= 0x99;
            *(u32 *)(s + field) = target;
            field += 0xAC;
            {
                u16 *hp = (u16 *)(s + (off = field));
                *hp = zero;
            }
            asm("" ::"r"(field));
        } while (0);
        {
            register u32 o asm("r0") = 0x114;
            register u32 *fl asm("r2");
            u32 m;
            asm("" : "+r"(o));
            fl = (u32 *)(s + o);
            m = ~1;
            *fl &= m;
        }
        *(u32 *)(s + 0x104) = zero;
    }
    if (!(u8)Callback_InvokeActor(s, *(void **)(s + 0x270))) {
        if (!sub_0812F3CC(s)) {
            u8 c = s[0x2B0];
            u32 *f = (u32 *)(s + 0x2D0);
            if (c)
                setf(f, 0x20);
            if (tst(f, 0x20)) {
                if (p[3]++ >= Route_GetCurrentPointNodeId(p)) {
                    SceneActorRoute_AdvanceActorPoint(s, p);
                    return;
                }
            }
            ++*(u32 *)(s + 0x2AC);
        }
    }
}
