// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern void ActorShadow_Disable(void *);
extern void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
extern void Golem_UpdateRevival(void);
#ifdef NONMATCHING
// fuzzy: 89.5%
// Remaining difference: flag pointer is copied to r6 after the load in the original; scratch register choice for the shadow offset.
static inline u8 take6030(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
struct Obj6030 {
    u32 flags;
    u8 pad[0x1C];
};
struct Part6030 {
    u8 pad[8];
    u32 flags;
};
void Golem_UpdateRevivalReturn(u8 *s, s32 arg, u32 unused)
{
    u32 queued;
    register u8 *flag asm("r6");
    {
        u32 mask = 1;
        if (s[8] == 0) {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            u32 value = obj->flags;
            obj->flags = value | mask;
        } else {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            struct Part6030 *part = (struct Part6030 *)((u8 *)obj + 0x20);
            u32 value = part->flags;
            part->flags = value | mask;
        }
    }
    queued = take6030(flag = s + 0x2B2);
    if (queued) {
        ActorShadow_Disable(s + 0x320);
    } else if (arg > 0x1A) {
        void (*fn)(void);
        u32 code;
        u32 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry(s, 7);
        fn = Golem_UpdateRevival;
        code = 0xD;
        *flag = 1;
        s[0x2B0] = queued;
        s[0x2B5] = code;
        *(void (**)(void))(s + 0x21C) = fn;
        *(u16 *)(s + 0x2C8) = queued;
        flags = (u32 *)(s + 0x114);
        mask = ~1;
        *flags &= mask;
        *(u32 *)(s + 0x104) = queued;
    } else {
        s[0x2B3] = queued;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Golem_UpdateRevivalReturn);
#endif
